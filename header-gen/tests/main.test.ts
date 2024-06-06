import { generateBlob, generateCArray, generateDsHexLiteral } from '../src/main';
import { MlModelHeader } from '../src/MlModelHeader';

describe('Test the blob and code generators', () => {
    const testData: Array<{
        headerData: MlModelHeader, expectedC: string, expectedDs: string
    }> = [
        {
            headerData: {
                samples_period: 20,
                samples_length: 750,
                sample_dimensions: 1,
                actions: [
                    { threshold: 0.6, label: "Jumping" },
                    { threshold: 0.7, label: "Running" },
                    { threshold: 0.8, label: "Standing" },
                    { threshold: 0.9, label: "Walking" }
                ]
            },
            expectedC: 'const uint32_t header_data[21] = {\n' +
                       '    0x4D4F444C, 0x00140054, 0x000102EE, 0x00000000, \n' +
                       '    0x04000000, 0x3F19999A, 0x6D754A08, 0x676E6970, \n' +
                       '    0x00000000, 0x3F333333, 0x6E755208, 0x676E696E, \n' +
                       '    0x00000000, 0x3F4CCCCD, 0x61745309, 0x6E69646E, \n' +
                       '    0x00000067, 0x3F666666, 0x6C615708, 0x676E696B, \n' +
                       '    0x00000000, \n' +
                '};\n',
            expectedDs: 'const headerBlob = hex`4C444F4D54001400EE02010000000000000000049A99193F084A756D70696E67000000003333333F0852756E6E696E6700000000CDCC4C3F095374616E64696E670000006666663F0857616C6B696E6700000000`;\n',
        }, {
            headerData: {
                samples_period: 25,
                samples_length: 80,
                sample_dimensions: 3,
                actions: [
                    { threshold: 0.8, label: "Shake" },
                    { threshold: 0.8, label: "Still" },
                    { threshold: 0.8, label: "Circle" }
                ]
            },
            expectedC: 'const uint32_t header_data[14] = {\n' +
                '    0x4D4F444C, 0x00190038, 0x00030050, 0x00000000, \n' +
                '    0x03000000, 0x3F4CCCCD, 0x61685306, 0x0000656B, \n' +
                '    0x3F4CCCCD, 0x69745306, 0x00006C6C, 0x3F4CCCCD, \n' +
                '    0x72694307, 0x00656C63, \n' +
                '};\n',
            expectedDs: 'const headerBlob = hex`4C444F4D38001900500003000000000000000003CDCC4C3F065368616B650000CDCC4C3F065374696C6C0000CDCC4C3F07436972636C6500`;\n',
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
