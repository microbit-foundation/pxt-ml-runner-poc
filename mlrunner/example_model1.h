/**
 * @brief An ML4F model example that can be used to test the MLRunner lib.
 *
 * @copyright
 * Copyright 2024 Micro:bit Educational Foundation.
 * SPDX-License-Identifier: MIT
 */
#pragma once

#if DEVICE_MLRUNNER_USE_EXAMPLE_MODEL == 1

/* This is a struct representation of the header included at the beginning of model_example
#include <mlrunner.h>
const ml_model_header_t ml4f_model_example_header = {
    .magic0 = MODEL_HEADER_MAGIC0,
    .header_size = 0x23,        // 35
    .model_offset = 0x24,       // 36
    .samples_length = 0x50,     // 80
    .sample_dimensions = 0x03,  // 3
    .reserved = { 0x00, 0x00, 0x00, 0x00 },
    .number_of_labels = 0x03,
    // 19 bytes + 1 extra null terminators at the end
    .labels = {
        "Shake\0"
        "Still\0"
        "Circle\0"
    }
};
*/

#define ml4f_model_example_header_len   36
#define ml4f_model_example_size         860
#define ml4f_full_model_size            (ml4f_model_example_header_len + ml4f_model_example_size)

const unsigned int example_model[ml4f_full_model_size] = {
    // Manually converted ml4f_model_example_header
    0x4D4F444C, 0x00240023, 0x00030050, 0x03000000,
    0x6B616853, 0x74530065, 0x006C6C69, 0x63726943,
    0x0000656C,

    // ML4F model
    0x30470f62, 0x46344c4d, 0x00000050, 0x00000d70, 0x000005a4, 0x00000000, 0x00000000, 0x000000a8, 
    0x00000008, 0x00000001, 0x00000008, 0x00000001, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
    0x00000018, 0x00000000, 0x00000003, 0x00000000, 0x5ff0e92d, 0x6901460f, 0x60391809, 0x60792100, 
    0x46036838, 0x1a1eecb3, 0x0208f207, 0x0a01ec92, 0x0a01ee20, 0x0a21ee30, 0x0a01eca2, 0x0a01ec92, 
    0x0a02ee20, 0x0a22ee30, 0x0a01eca2, 0x0a01ec92, 0x0a03ee20, 0x0a23ee30, 0x0a01eca2, 0x0a01ec92, 
    0x0a04ee20, 0x0a24ee30, 0x0a01eca2, 0x0a01ec92, 0x0a05ee20, 0x0a25ee30, 0x0a01eca2, 0x0a01ec92, 
    0x0a06ee20, 0x0a26ee30, 0x0a01eca2, 0x0a01ec92, 0x0a07ee20, 0x0a27ee30, 0x0a01eca2, 0x0a01ec92, 
    0x0a08ee20, 0x0a28ee30, 0x0a01eca2, 0x0a01ec92, 0x0a09ee20, 0x0a29ee30, 0x0a01eca2, 0x0a01ec92, 
    0x0a0aee20, 0x0a2aee30, 0x0a01eca2, 0x0a01ec92, 0x0a0bee20, 0x0a2bee30, 0x0a01eca2, 0x0a01ec92, 
    0x0a0cee20, 0x0a2cee30, 0x0a01eca2, 0x0a01ec92, 0x0a0dee20, 0x0a2dee30, 0x0a01eca2, 0x0a01ec92, 
    0x0a0eee20, 0x0a2eee30, 0x0a01eca2, 0x0a01ec92, 0x0a0fee20, 0x0a2fee30, 0x0a01eca2, 0x0224f202, 
    0x1a12ecb3, 0x0244f207, 0x0a01ec92, 0x0a01ee20, 0x0a21ee30, 0x0a01eca2, 0x0a01ec92, 0x0a02ee20, 
    0x0a22ee30, 0x0a01eca2, 0x0a01ec92, 0x0a03ee20, 0x0a23ee30, 0x0a01eca2, 0x0a01ec92, 0x0a04ee20, 
    0x0a24ee30, 0x0a01eca2, 0x0a01ec92, 0x0a05ee20, 0x0a25ee30, 0x0a01eca2, 0x0a01ec92, 0x0a06ee20, 
    0x0a26ee30, 0x0a01eca2, 0x0a01ec92, 0x0a07ee20, 0x0a27ee30, 0x0a01eca2, 0x0a01ec92, 0x0a08ee20, 
    0x0a28ee30, 0x0a01eca2, 0x0a01ec92, 0x0a09ee20, 0x0a29ee30, 0x0a01eca2, 0x023cf202, 0xf2006838, 
    0xf20703c0, 0x24100268, 0x0a01ecb3, 0x0108f207, 0x0a0eecf1, 0x7a0eecf3, 0x0aa7ee60, 0x1a08ee21, 
    0x0a20ee30, 0x1aa8ee61, 0x0a01ee30, 0x2a09ee22, 0x0a21ee30, 0x2aa9ee62, 0x0a02ee30, 0x3a0aee23, 
    0x0a22ee30, 0x3aaaee63, 0x0a03ee30, 0x4a0bee24, 0x0a23ee30, 0x4aabee64, 0x0a04ee30, 0x5a0cee25, 
    0x0a24ee30, 0x5aacee65, 0x0a05ee30, 0x6a0dee26, 0x0a25ee30, 0x6aadee66, 0x0a06ee30, 0x7a0eee27, 
    0x0a26ee30, 0x0a07ee30, 0x0a0aecf1, 0x7a0aecf3, 0x0aa7ee60, 0x1a08ee21, 0x0a20ee30, 0x1aa8ee61, 
    0x0a01ee30, 0x2a09ee22, 0x0a21ee30, 0x2aa9ee62, 0x0a02ee30, 0x3a0aee23, 0x0a22ee30, 0x3aaaee63, 
    0x0a03ee30, 0x4a0bee24, 0x0a23ee30, 0x4aabee64, 0x0a04ee30, 0x5a0cee25, 0x0a24ee30, 0x0a05ee30, 
    0x0a01eca2, 0xd18f3c01, 0x0268f207, 0x28006810, 0xf240bfb8, 0xc2010000, 0x28006810, 0xf240bfb8, 
    0xc2010000, 0x28006810, 0xf240bfb8, 0xc2010000, 0x28006810, 0xf240bfb8, 0xc2010000, 0x28006810, 
    0xf240bfb8, 0xc2010000, 0x28006810, 0xf240bfb8, 0xc2010000, 0x28006810, 0xf240bfb8, 0xc2010000, 
    0x28006810, 0xf240bfb8, 0xc2010000, 0x28006810, 0xf240bfb8, 0xc2010000, 0x28006810, 0xf240bfb8, 
    0xc2010000, 0x28006810, 0xf240bfb8, 0xc2010000, 0x28006810, 0xf240bfb8, 0xc2010000, 0x28006810, 
    0xf240bfb8, 0xc2010000, 0x28006810, 0xf240bfb8, 0xc2010000, 0x28006810, 0xf240bfb8, 0xc2010000, 
    0x28006810, 0xf240bfb8, 0xc2010000, 0xf2006838, 0xf2077300, 0x24030208, 0x0a01ecb3, 0x0168f207, 
    0x0a0eecf1, 0x7a0eecf3, 0x0aa7ee60, 0x1a08ee21, 0x0a20ee30, 0x1aa8ee61, 0x0a01ee30, 0x2a09ee22, 
    0x0a21ee30, 0x2aa9ee62, 0x0a02ee30, 0x3a0aee23, 0x0a22ee30, 0x3aaaee63, 0x0a03ee30, 0x4a0bee24, 
    0x0a23ee30, 0x4aabee64, 0x0a04ee30, 0x5a0cee25, 0x0a24ee30, 0x5aacee65, 0x0a05ee30, 0x6a0dee26, 
    0x0a25ee30, 0x6aadee66, 0x0a06ee30, 0x7a0eee27, 0x0a26ee30, 0x0a07ee30, 0x0a02ecf1, 0x7a02ecf3, 
    0x0aa7ee60, 0x1a08ee21, 0x0a20ee30, 0x0a01ee30, 0x0a01eca2, 0xd1af3c01, 0x0208f207, 0x21034610, 
    0xf802f000, 0x9ff0e8bd, 0xb5382901, 0x2a00edd0, 0x1d03d929, 0x0281eb00, 0x7a01ecf3, 0x7a62eef4, 
    0xfa10eef1, 0xeef0bfc8, 0x429a2a67, 0x2400d1f4, 0x4a10ee02, 0xed954605, 0xee300a00, 0xf0000a62, 
    0x3401f815, 0xee3242a1, 0xeca52a00, 0xd8f20a01, 0xed902300, 0xeec77a00, 0x33017a02, 0xece04299, 
    0xd8f67a01, 0x2900bd38, 0xbd38d1e1, 0x7a29eddf, 0x0ae7eeb4, 0xfa10eef1, 0xeddfd448, 0xeeb47a27, 
    0xeef10ae7, 0xdc3afa10, 0x7a24eddf, 0x4a24eddf, 0x3a24ed9f, 0x3a24eddf, 0x5a24ed9f, 0x4a24ed9f, 
    0x5a24eddf, 0x6a24ed9f, 0x6a24eddf, 0x7a27ee60, 0x7a00eeb7, 0x7aa4ee77, 0x7ae4ee77, 0x0a83eea7, 
    0x4ae7eefd, 0x0aa3eea7, 0x7a45eef0, 0x7a00eee4, 0x3a90ee14, 0x5a80eee7, 0x6a80eea5, 0x6a00eee6, 
    0x7a66eef0, 0x6a47eef0, 0x6a80eee7, 0x7a80eea6, 0x2a10ee17, 0x53c3eb02, 0x3a10ee00, 0xeddf4770, 
    0xeeb77a10, 0xee877a00, 0x47700a27, 0x0a0ced9f, 0x00004770, 0xc2aa0000, 0x42ae0000, 0x3fb8aa3b, 
    0x4b400000, 0xbf317200, 0xb5bfbe8e, 0x3c092f6e, 0x3ab4a000, 0x3d2aadad, 0x3e2aaa28, 0x3efffffb, 
    0x00000000, 0x3f998896, 0xbea8a6d7, 0x3f835e28, 0xbf35a4c4, 0x3fe346c2, 0x3ea3418e, 0x40128dd4, 
    0x3f2cd9a8, 0x4012c5f8, 0xbf54edd5, 0x3fea3250, 0x3f6f4c3a, 0x3f8b44e8, 0x3f579f46, 0x3f969087, 
    0xbe16333b, 0x3fa53ff2, 0x3f8dba1f, 0x3fe1188f, 0xbf10df9d, 0x3fc850eb, 0xbed0b83e, 0x3fab2d04, 
    0xbef1fd68, 0x3efab87f, 0xbfa1d47b, 0x3e803c5e, 0xbf20272b, 0x3f0b9f74, 0xbfb9afab, 0x3d418ead, 
    0xbfd66f61, 0x3d4332bc, 0xbfe8093d, 0x3d6ac6d2, 0xc03ce104, 0x3fe6c43e, 0x3d98a86b, 0x3ffea627, 
    0x3d7bde2e, 0x40a1e665, 0x3e8fc697, 0x4018348f, 0xbf872b1d, 0x3ffddce4, 0xbf5e5cf7, 0x3fe7c66c, 
    0xbf753986, 0xbdeb3612, 0xbe9319a1, 0x3e3101d0, 0x3ce600d8, 0x3d281000, 0x3e0552e6, 0x3e2dd96b, 
    0x3e00f462, 0xbc513b10, 0xbe4bbd66, 0x3eb9b973, 0xbd211358, 0x3e2068d4, 0xbdbfbf23, 0x3d02b8ad, 
    0xbd316931, 0x3db8097d, 0x3e7e64ee, 0xbd80abca, 0xbe0c3b99, 0xbe3ae31c, 0xbe69f271, 0x3e4d1b07, 
    0x3dd7ff99, 0xbd80ee12, 0xbe03a290, 0x3da56ae0, 0x3d9a9626, 0xbf86b2f1, 0xbf27b842, 0x3ee9df6c, 
    0xbe55b401, 0xbddfb7e7, 0x3e9ffe01, 0x3da56f9c, 0x3ebef265, 0x3e24313c, 0x3e949a2b, 0x3e814129, 
    0xbd37bc20, 0x3e870237, 0xbe8fc671, 0xbe729456, 0x3e93ef7b, 0xbed03a39, 0x3e9b247c, 0xbf643dfa, 
    0xbef6f381, 0xbdca0513, 0x3d801445, 0xbe2e5473, 0x3eb2fcdc, 0xbbe5ce40, 0xbf01589b, 0xbe18b7ad, 
    0xbe36b849, 0x3d9f6f95, 0xbe632f23, 0x3d931a5f, 0x3d7e8358, 0xbed88bcd, 0x3da37da9, 0xbe519539, 
    0xbe9462e9, 0x3de218ce, 0xbe635385, 0xbb1fa322, 0x3e89905f, 0x3d833575, 0x3e60aed2, 0xbe9e0067, 
    0xbcd49ee9, 0xbda0dff2, 0x3e3861d4, 0x3da1ed87, 0xbe032ec4, 0x3ddaa031, 0xbe9c4af9, 0xbebd5ad8, 
    0x3d82d480, 0x3ea0acea, 0x3ec3f0f2, 0x3e971514, 0x3cd0e51d, 0x3d558514, 0x3ead7d7e, 0xbeeade35, 
    0x3ea76fd8, 0x3da3668e, 0xbd806bd7, 0xbdb07c44, 0x3e48feac, 0x3db3d306, 0x3e6590b5, 0x3ebf6517, 
    0x3eae18ea, 0xbdda0f42, 0x3e98f46b, 0x3d0fc07c, 0xbe6a9463, 0xbddac639, 0x3dbba97f, 0x3d018dea, 
    0x3de955a1, 0x3ea172f6, 0xbe4a007f, 0x3e393621, 0x3ec84974, 0xbd62361c, 0x3ef01468, 0x3ea77779, 
    0x3df84742, 0x3e8661b9, 0xbeba6688, 0x3e2da6b9, 0xbe8a4434, 0x3de5fa95, 0xbdf1226c, 0x3c30e180, 
    0xbcc7052e, 0x3d46852f, 0xbe6b5885, 0x3dcbcbef, 0xbe9f39a9, 0x3d364982, 0x3d6e9b98, 0x3e3e9499, 
    0x3cd938dd, 0x3ec2965f, 0xbe32d03b, 0x3e593e80, 0xbe01d7bb, 0xbea7330c, 0xbe129d46, 0xbd9dd87c, 
    0x3eaf22c4, 0xbeebd736, 0xbda30491, 0x3d55177a, 0xbc80a926, 0x3e450cc4, 0x3ed550dc, 0xbe33a4a7, 
    0x3e12492a, 0xbe0326b6, 0x3eaeda38, 0x3f42f659, 0x3e9c14db, 0x3d9543cd, 0x3dbf83e1, 0xbe172ae9, 
    0xbdbfb7db, 0xbe64414d, 0x3e537942, 0xbdd49d06, 0x3e972d42, 0x3e963492, 0xbe80a6fa, 0x3d5f2e9f, 
    0x3e5d8ea6, 0xbe9b42b5, 0x3e1323e7, 0x3e142710, 0x3d812bbf, 0x3d7ab0bd, 0xbe8070dd, 0xbd2234b1, 
    0x3e6da425, 0xbe531cac, 0x3d95bb4c, 0x3e362c42, 0x3ea855b5, 0x3e71edf8, 0x3dd378be, 0xbecce473, 
    0xbcab067a, 0x3e8df3a9, 0xbd063b64, 0x3e949304, 0xbd093232, 0x3ec2dd2f, 0x3ecb250d, 0xbc593410, 
    0xbe8a56a9, 0xbe9d3e8e, 0x3e746b7c, 0xbee17cc5, 0x3e9fb64b, 0x3e837d4e, 0xbd42cddb, 0xbe70e71e, 
    0x3eca4fd9, 0xbd9f941a, 0xbd4a1c21, 0x3e984341, 0x3d1fecbb, 0x3f63a9c4, 0x3f25d861, 0x3e19fe28, 
    0x3e46b189, 0xbcd88572, 0x3db5d134, 0x3e88b8cc, 0x3e8d1f40, 0xbd9bb897, 0xbd770f3a, 0x3dfa42ce, 
    0xbe4e3916, 0x3e12a9a8, 0x3e26f27e, 0xbe771fe7, 0xbd3f1c1f, 0x3e195f3b, 0xbe5f3e7a, 0xbe1deea8, 
    0x3da1832c, 0xbdf74b3f, 0x3ea4c435, 0xbe819223, 0x3e977a90, 0x3df8f0b7, 0xbe8e4d67, 0x3eab76cb, 
    0xbdae9056, 0x3e084793, 0x3e12bf7e, 0xbeb466aa, 0x3d992000, 0x3e029b63, 0xbf3824ba, 0x3f398413, 
    0xbd6dc18a, 0xbe9334db, 0x3dc72171, 0x3dd08057, 0xbd9b597f, 0x3dd5cadb, 0x3e96cffd, 0x3f182507, 
    0xbe35989d, 0x3f0e0d59, 0x3d02d65c, 0xbee8b321, 0xbef3ea20, 0xbb404a5e, 0xbecd9516, 0xbf871d38, 
    0x3e645bd3, 0xbea1b859, 0xbe7ca3a1, 0x3d900d60, 0xbd8abcca, 0x3d93fc71, 0x3d8da531, 0xbde87a5a, 
    0xbe532e39, 0xbead3cb9, 0x3e1766ad, 0xbf0a8d97, 0xbe451bdd, 0xbea30dbb, 0x3d88c926, 0x3d38ad6d, 
    0xbea42f39, 0xbe2fd81c, 0xbf168af0, 0xbe9b5ac7, 0x3f0ff30e, 0x3e8a0e8c, 0x3e8b09ad, 0x3e136ffe, 
    0x3e8ac5fe, 0xbee48f99, 0x3efd6479, 0x3eb61d83, 0xbe49373a, 0xbea445a1, 0xbe4cca4b, 0xbc4ff2b0, 
    0x3ef9ce68, 0xbe8c756b, 0x3d033f17, 0x3eb17a66, 0x3d5b8439, 0x3eb1b069, 0xbf164646, 0xbe6affa2, 
    0xbe8eb6a8, 0xbed45835, 0xbe8038cb, 0xbead0a9c, 0xbe2fe1fd, 0x3e041191, 0xbcbb76ef, 0xbe92103c, 
    0x3d4e3246, 0x3ea42da9, 0x3d31b888, 0xbce8405c, 0xbed9de9e, 0xbd96e8e7, 0x3dbdc93c, 0x3e5ca019, 
    0x3e321a97, 0x3ea2306b, 0xbe142f7b, 0xbe194927, 0xbd9e6073, 0xbe524970, 0x3eb01c54, 0xbea48307, 
    0x3e6aaa05, 0x3d9e9db8, 0x3e20b490, 0x3d5665ca, 0x3e21198e, 0xbd0fb05f, 0x3dc5071c, 0x3e71b213, 
    0xbe589e0c, 0x3e6c4d20, 0x3e93acf4, 0x3e183fe5, 0x3e851e4e, 0x3e45f6da, 0xbe177b31, 0xbba00bd0, 
    0xbe426f89, 0xbdcc9e05, 0x3e765865, 0xbed07d8b, 0x3e05a43b, 0x3e59ab4c, 0x3ca93bfc, 0x3edbb6bc, 
    0xbe9b51a3, 0xbe329829, 0xbe2cc08e, 0xbef423f4, 0xbe598210, 0xbf3631c0, 0xbe842ba3, 0xbd9d680c, 
    0xbd87ffb0, 0xbe544206, 0x3d59ea52, 0x3ed1eb66, 0xbe462f58, 0x3ed36a67, 0xbc19f127, 0x3dbf76e2, 
    0x3f09af4f, 0x3deae444, 0x3eb5197c, 0xbc71d6a1, 0xbdcb6793, 0xbc6ec66e, 0x3e11b82c, 0xbe5fb26e, 
    0xbe86719b, 0xbe422f22, 0x3e808760, 0x3e17fc4c, 0x3d96d5f0, 0x3d961541, 0x3db77a47, 0x3d4fe03e, 
    0x3e93b9cd, 0xbeb7526e, 0x3e700b52, 0x3e744a3e, 0x3f4de520, 0x3e90f74f, 0x3ece8d1b, 0x3ebd4c77, 
    0xbd700c01, 0x3e282fe2, 0xbe29cb4b, 0xbed36916, 0xbe08e58e, 0xbe6323bb, 0xbf202ccd, 0xbd8cd190, 
    0x3e307539, 0xbef57841, 0x3e91d10a, 0x3e73bd73, 0x3e9ecbe8, 0x3e829e9a, 0x3e89f577, 0x3e84c80f, 
    0xbe4d9ad5, 0xbf5054e2, 0x3f1be54e, 0xbd80a15c, 0x3f1f08a2, 0xbf10a851, 0xbe347a79, 0xbefd8fc0, 
    0x3eff4fd9, 0xbd6fb6a2, 0xbd6c4414, 0xbf87a057, 0xbeb28229, 0xbe09c0a8, 0xbf1b375b, 0x3f27930e, 
    0xbdd8a3b3, 0x3ef554d6, 0xbea366db, 0xbd6c61ee, 0x3eeebac8, 0xbf5ceda8, 0x3ea833c9, 0xbeed45a0, 
    0x3f0450ef, 0x3d08c65a, 0xbe472ec0, 0x3be847a0, 0x3edd752c, 0x3ea37272, 0xbdbe0f8c, 0x3e6ce47a, 
    0xbe904c11, 0x3da09318, 0xbdfc646c, 0xbecc99e2, 0xbf23b26a, 0xbda19859, 0x3f2e9e48, 0xbee31a37, 
    0x3f274e40, 0xbf30d0fa, 0xbf03b1b3, 0x3e818e33, 0x3dcf667e, 0x3eba6b0d, 0xbe32cd4e, 0xbe945657, 
    0x3c808420, 0xbecaf6e6, 0x3f06a6ed, 0xbf37e27b, 0xbde10673, 0x3f42f270, 0xbf7483d8, 0xbe4bfc54, 
    0xbdabf2b9, 0xbe9ba531, 0xbe19dcb6, 0x3fa0d82b, 
};

#endif // DEVICE_MLRUNNER_USE_EXAMPLE_MODEL
