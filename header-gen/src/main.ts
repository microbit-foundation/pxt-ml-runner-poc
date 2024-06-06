/**
 * (c) 2024, Micro:bit Educational Foundation and contributors.
 *
 * SPDX-License-Identifier: MIT
 * 
 * This script converts a TS object matching the ml_model_header_t C struct
 * format into a binary blob to be included on top of the ML4F model.
 * 
 * The current script has been created to match the following struct format
 * from mlrunner/mlrunner.h. If the format is different this script will need
 * to be updated.
 *
 * typedef struct ml_action_s {
 *     const float threshold;
 *     const uint8_t label_length;
 *     const char label[0];
 * } ml_action_t;
 *
 * typedef struct ml_model_header_t {
 *     uint32_t magic0;
 *     uint16_t header_size;
 *     uint16_t model_offset;
 *     uint16_t samples_period;
 *     uint16_t samples_length;
 *     uint16_t sample_dimensions;
 *     uint8_t reserved[6];
 *     const uint8_t number_of_actions;
 *     const ml_action_t actions[0];
 * } ml_model_header_t;
 */
import { MlModelHeader } from './MlModelHeader';

const HEADER_MAGIC = 0x4D4F444C;
const CONST_SIZES = {
    magic0: 4,
    header_size: 2,
    model_offset: 2,
    samples_period: 2,
    samples_length: 2,
    sample_dimensions: 1,
    reserved: 6,
    number_of_actions: 1,
};
const ACTION_HEADER_SIZES = {
    threshold: 4,
    label_length: 1,
    label: 1,       // This is a placeholder for the null terminator
};

/**
 * Generate a binary blob from a MlModelHeader object.
 *
 * This format is used to prepend the model binary blob in the ML4F model and
 * it mirrors the ml_model_header_t struct from mlrunner/mlrunner.h.
 *
 * @param data The MlModelHeader object to convert to a binary blob.
 * @returns The binary blob as an ArrayBuffer.
 */
export function generateBlob(data: MlModelHeader): ArrayBuffer {
    // Calculate size of the actions and labels within, including null terminators and padding
    const fixedActionSize = Object.values(ACTION_HEADER_SIZES).reduce((acc, size) => acc + size, 0);
    let structPadding = 0;
    let actionsSize = data.actions.reduce((acc, action) => {
        const actionStructSize = fixedActionSize + action.label.length;
        // Each action struct is aligned to 4 bytes
        structPadding = (actionStructSize % 4 === 0) ? 0 : 4 - (actionStructSize % 4);
        return acc + actionStructSize + structPadding;
    }, 0);
    // The last padding is not needed
    actionsSize -= structPadding;

    // Header size  = fixed size values + size of all actions
    const fixedHeaderSize = Object.values(CONST_SIZES).reduce((acc, size) => acc + size, 0);
    const headerSize = fixedHeaderSize + actionsSize;

    // The model offset is aligned to 4 bytes, as the actions are also aligned
    // to 4 bytes, we can check the alignment is already satisfied
    const headerPadding = (headerSize % 4 === 0) ? 0 : 4 - (headerSize % 4);
    const modelOffset = headerSize + headerPadding;

    const buffer = new ArrayBuffer(modelOffset);
    const view = new DataView(buffer);
    let offset = 0;

    offset = addToView(view, offset, HEADER_MAGIC, CONST_SIZES.magic0);
    offset = addToView(view, offset, headerSize, CONST_SIZES.header_size);
    offset = addToView(view, offset, modelOffset, CONST_SIZES.model_offset);
    offset = addToView(view, offset, data.samples_period, CONST_SIZES.samples_period);
    offset = addToView(view, offset, data.samples_length, CONST_SIZES.samples_length);
    offset = addToView(view, offset, data.sample_dimensions, CONST_SIZES.sample_dimensions);

    // Add reserved bytes as zeros
    for (let i = 0; i < CONST_SIZES.reserved; i++) {
        offset = addToView(view, offset, 0, 1);
    }

    offset = addToView(view, offset, data.actions.length, CONST_SIZES.number_of_actions);

    // Add action structures
    data.actions.forEach(action => {
        const start_offset = offset;
        view.setFloat32(offset, action.threshold, true);
        offset += ACTION_HEADER_SIZES.threshold;
        offset = addToView(view, offset, action.label.length + 1, ACTION_HEADER_SIZES.label_length);
        for (let i = 0; i < action.label.length; i++) {
            offset = addToView(view, offset, action.label.charCodeAt(i), 1);
        }
        offset = addToView(view, offset, 0, 1); // null terminator
        // Align to 4 bytes and pad with zeros
        const actionStructSize = offset - start_offset;
        const extraBytes = (actionStructSize % 4 === 0) ? 0 : 4 - (actionStructSize % 4);
        for (let i = 0; i < extraBytes; i++) {
            offset = addToView(view, offset, 0, 1);
        }
    });

    return buffer;
}

/**
 * Add a value to a DataView at a given offset.
 *
 * @param view The DataView to add the value to.
 * @param offset The offset to add the value at.
 * @param value The value to add.
 * @param size The size of the value in bytes.
 * @returns The new offset after adding the value.
 */
function addToView(view: DataView, offset: number, value: number, size: number): number {
    switch (size) {
        case 1:
            view.setUint8(offset, value);
            break;
        case 2:
            view.setUint16(offset, value, true);
            break;
        case 4:
            view.setUint32(offset, value, true);
            break;
        default:
            throw new Error('Invalid size');
    }
    return offset + size;
}

/**
 * Generate a C array from a binary blob.
 *
 * @param blob The binary blob to convert to a C array.
 * @returns The C array as a string.
 */
export function generateCArray(blob: ArrayBuffer): string {
    const data32 = new Uint32Array(blob);
    let cCode = `const uint32_t header_data[${data32.length}] = {`;
    for (let i = 0; i < data32.length; i++) {
        // Each line to 4 32-bit values
        if (i % 4 === 0) {
            cCode += '\n    ';
        }
        cCode += `0x${data32[i].toString(16).padStart(8, '0').toUpperCase()}, `;
    }
    cCode += '\n};\n';
    return cCode;
}

/**
 * Generate a DeviceScript hex template literal from a binary blob.
 *
 * @param blob The binary blob to convert to a hex literal.
 * @returns The hex template literal as a "normal" string.
 */
export function generateDsHexLiteral(blob: ArrayBuffer): string {
    const data8 = new Uint8Array(blob);
    let hex = 'const headerBlob = hex`';
    for (let i = 0; i < data8.length; i++) {
        hex += `${data8[i].toString(16).padStart(2, '0').toUpperCase()}`;
    }
    hex += '`;\n';
    return hex;
}
