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
 * typedef struct ml_model_header_t {
 *     uint32_t magic0;
 *     uint16_t header_size;
 *     uint16_t model_offset;
 *     uint16_t samples_length;
 *     uint16_t sample_dimensions;
 *     uint8_t reserved[4];
 *     uint8_t number_of_labels;
 *     char labels[];
 * } ml_model_header_t;
 */
import { headerData } from './inputdata';
import { MlModelHeader } from './MlModelHeader';

const HEADER_MAGIC = 0x4D4F444C;
const CONST_SIZES = {
    magic0: 4,
    header_size: 2,
    model_offset: 2,
    samples_length: 2,
    sample_dimensions: 1,
    reserved: 4,
    number_of_labels: 1,
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
    // Calculate the size of the labels part including null terminators
    const labelsSize = data.labels.reduce((acc, label) => acc + label.length + 1, 0);

    // Header size  = fixed size values + size of all labels including null terminators
    const fixedSize = Object.values(CONST_SIZES).reduce((acc, size) => acc + size, 0);
    const headerSize = fixedSize + labelsSize;

    // The model offset is aligned to 4 bytes
    const headerPadding = headerSize % 4 === 0 ? 0 : 4 - (headerSize % 4);
    const modelOffset = headerSize + headerPadding;

    const buffer = new ArrayBuffer(modelOffset);
    const view = new DataView(buffer);
    let offset = 0;

    offset = addToView(view, offset, HEADER_MAGIC, CONST_SIZES.magic0);
    offset = addToView(view, offset, headerSize, CONST_SIZES.header_size);
    offset = addToView(view, offset, modelOffset, CONST_SIZES.model_offset);
    offset = addToView(view, offset, data.samples_length, CONST_SIZES.samples_length);
    offset = addToView(view, offset, data.sample_dimensions, CONST_SIZES.sample_dimensions);

    // Add reserved bytes as zeros
    for (let i = 0; i < CONST_SIZES.reserved; i++) {
        offset = addToView(view, offset, 0, 1);
    }

    offset = addToView(view, offset, data.labels.length, CONST_SIZES.number_of_labels);

    // Add labels as a null-terminated strings
    data.labels.forEach(label => {
        for (let i = 0; i < label.length; i++) {
            offset = addToView(view, offset, label.charCodeAt(i), 1);
        }
        offset = addToView(view, offset, 0, 1); // null terminator
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
