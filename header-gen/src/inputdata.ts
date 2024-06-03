/**
 * (c) 2024 Micro:bit Educational Foundation and contributors.
 *
 * SPDX-License-Identifier: MIT
 */
import { MlModelHeader } from './MlModelHeader';

export const headerData: MlModelHeader = {
    samples_period: 25,
    samples_length: 80,
    sample_dimensions: 3,
    labels: ["Shake", "Still", "Circle"]
};
