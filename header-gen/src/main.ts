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
 *     uint8_t reserved[5];
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
    reserved: 5,
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

    // uint32_t magic0
    view.setUint32(offset, HEADER_MAGIC, true);
    offset += CONST_SIZES.magic0;

    // uint16_t header_size
    view.setUint16(offset, headerSize, true);
    offset += CONST_SIZES.header_size;

    // uint16_t model_offset
    view.setUint16(offset, modelOffset, true);
    offset += CONST_SIZES.model_offset;

    // uint16_t samples_length
    view.setUint16(offset, data.samples_length, true);
    offset += CONST_SIZES.samples_length;

    // uint8_t reserved[]
    for (let i = 0; i < CONST_SIZES.reserved; i++) {
        view.setUint8(offset, 0);
        offset += 1;
    }

    // uint8_t number_of_labels
    view.setUint8(offset, data.labels.length);
    offset += CONST_SIZES.number_of_labels;

    // Add labels as a null-terminated strings
    data.labels.forEach(label => {
        for (let i = 0; i < label.length; i++) {
            view.setUint8(offset, label.charCodeAt(i));
            offset += 1;
        }
        view.setUint8(offset, 0); // null terminator
        offset += 1;
    });

    return buffer;
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


if (require.main === module) {
    const headerBlob = generateBlob(headerData);
    console.log('Blob:', headerBlob);
    console.log(`\nC code:\n${generateCArray(headerBlob)}`);
    console.log(`DeviceScript:\n${generateDsHexLiteral(headerBlob)}`);
}
