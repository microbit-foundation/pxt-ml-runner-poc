import { headerData } from "./inputdata";
import { generateBlob, generateCArray, generateDsHexLiteral } from "./main";

if (require.main === module) {
    const headerBlob = generateBlob(headerData);
    console.log('Blob:', headerBlob);
    console.log(`\nC code:\n${generateCArray(headerBlob)}`);
    console.log(`DeviceScript:\n${generateDsHexLiteral(headerBlob)}`);
}
