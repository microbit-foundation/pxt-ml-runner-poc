import { generateBlob, generateCArray, generateDsHexLiteral } from '../src/main';
import { MlModelHeader } from '../src/MlModelHeader';

describe('Test the blob and code generators', () => {
    const testData: Array<{
        headerData: MlModelHeader, expectedC: string, expectedDs: string
    }> = [
        {
            headerData: {
                samples_period: 20,
                samples_length: 250,
                sample_dimensions: 3,
                labels: ["Jumping", "Running", "Standing", "Walking"]
            },
            expectedC: 'const uint32_t header_data[14] = {\n' +
                       '    0x4D4F444C, 0x00380035, 0x00FA0014, 0x00000003, \n' +
                       '    0x04000000, 0x706D754A, 0x00676E69, 0x6E6E7552, \n' +
                       '    0x00676E69, 0x6E617453, 0x676E6964, 0x6C615700, \n' +
                       '    0x676E696B, 0x00000000, \n' +
                '};\n',
            expectedDs: 'const headerBlob = hex`4C444F4D350038001400FA0003000000000000044A756D70696E670052756E6E696E67005374616E64696E670057616C6B696E6700000000`;\n',
        }, {
            headerData: {
                samples_period: 25,
                samples_length: 80,
                sample_dimensions: 3,
                labels: ["Shake", "Still", "Circle"]
            },
            expectedC: 'const uint32_t header_data[10] = {\n' +
                '    0x4D4F444C, 0x00280027, 0x00500019, 0x00000003, \n' +
                '    0x03000000, 0x6B616853, 0x74530065, 0x006C6C69, \n' +
                '    0x63726943, 0x0000656C, \n' +
                '};\n',
            expectedDs: 'const headerBlob = hex`4C444F4D270028001900500003000000000000035368616B65005374696C6C00436972636C650000`;\n',
    }
        
    ];

    testData.forEach(({ headerData, expectedC, expectedDs }) => {
        it('should generate correct C code from the generated blob', () => {
            const blob = generateBlob(headerData);

            const cCode = generateCArray(blob);

            expect(cCode).toBe(expectedC);
        });
    });
    testData.forEach(({ headerData, expectedC, expectedDs }) => {
        it('should generate correct DeviceScript code from the generated blob', () => {
            const blob = generateBlob(headerData);

            const dsCode = generateDsHexLiteral(blob);

            expect(dsCode).toBe(expectedDs);
        });
    });
});
