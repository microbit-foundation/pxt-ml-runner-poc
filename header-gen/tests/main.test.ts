import { generateBlob, generateCArray, generateDsHexLiteral } from '../src/main';
import { MlModelHeader } from '../src/MlModelHeader';

describe('Test the blob and code generators', () => {
    const testData: Array<{
        headerData: MlModelHeader, expectedC: string, expectedDs: string
    }> = [
        {
            headerData: {
                samples_length: 250,
                sample_dimensions: 3,
                labels: ["Jumping", "Running", "Standing", "Walking"]
            },
            expectedC: 'const uint32_t header_data[13] = {\n' +
                       '    0x4D4F444C, 0x00340031, 0x000300FA, 0x04000000, \n' +
                       '    0x706D754A, 0x00676E69, 0x6E6E7552, 0x00676E69, \n' +
                       '    0x6E617453, 0x676E6964, 0x6C615700, 0x676E696B, \n' +
                       '    0x00000000, \n' +
                '};\n',
            expectedDs: 'const headerBlob = hex`4C444F4D31003400FA000300000000044A756D70696E670052756E6E696E67005374616E64696E670057616C6B696E6700000000`;\n',
        }, {
            headerData: {
                samples_length: 80,
                sample_dimensions: 3,
                labels: ["Shake", "Still", "Circle"]
            },
            expectedC: 'const uint32_t header_data[9] = {\n' +
                '    0x4D4F444C, 0x00240023, 0x00030050, 0x03000000, \n' +
                '    0x6B616853, 0x74530065, 0x006C6C69, 0x63726943, \n' +
                '    0x0000656C, \n' +
                '};\n',
            expectedDs: 'const headerBlob = hex`4C444F4D2300240050000300000000035368616B65005374696C6C00436972636C650000`;\n',
    }
        
    ];

    testData.forEach(({ headerData, expectedC, expectedDs }) => {
        it('should generate correct blob, C, and DeviceScript code', () => {
            const blob = generateBlob(headerData);
            const cCode = generateCArray(blob);
            const dsCode = generateDsHexLiteral(blob);

            expect(cCode).toBe(expectedC);
            expect(dsCode).toBe(expectedDs);
        });
    });
});
