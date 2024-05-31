/**
 * (c) 2024 Micro:bit Educational Foundation and contributors.
 *
 * SPDX-License-Identifier: MIT
 */
export type MlModelHeader = {
    samples_length: number;
    sample_dimensions: number;
    labels: string[];
};
