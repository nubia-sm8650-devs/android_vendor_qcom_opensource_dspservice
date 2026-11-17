/*
 * SPDX-FileCopyrightText: The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

package vendor.qti.hardware.dsp;

/**
 * Opens FastRPC device nodes on behalf of clients that may use a node but
 * may not open it themselves.
 */
@VintfStability
interface IDspService {
    /**
     * Opens the FastRPC device node of a DSP for the caller.
     *
     * @param domain FastRPC domain ID of the DSP. Only the CDSP (3) is
     *     served, as by the downstream service.
     * @return the opened device node, which the caller owns.
     * @throws EX_ILLEGAL_ARGUMENT for any other domain, or a service
     *     specific error with the errno of a failed open.
     */
    ParcelFileDescriptor openSession(int domain);
}
