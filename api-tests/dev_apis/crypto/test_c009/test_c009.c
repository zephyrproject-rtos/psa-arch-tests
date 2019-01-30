/** @file
 * Copyright (c) 2018, Arm Limited or its affiliates. All rights reserved.
 * SPDX-License-Identifier : Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
**/

#ifdef NONSECURE_TEST_BUILD
#include "val_interfaces.h"
#include "val_target.h"
#else
#include "val/common/val_client_defs.h"
#include "val/spe/val_partition_common.h"
#endif

#include "test_c009.h"
#include "test_data.h"
#include "val_crypto.h"

client_test_t test_c009_crypto_list[] = {
    NULL,
    psa_set_key_lifetime_test,
    psa_set_key_lifetime_negative_test,
    NULL,
};

static int g_test_count = 1;

int32_t psa_set_key_lifetime_test(security_t caller)
{
    uint8_t             *key_data, data[BUFFER_SIZE];
    psa_key_policy_t    policy;
    int                 num_checks = sizeof(check1)/sizeof(check1[0]);
    uint32_t            i, length;
    int32_t             status;
    boot_state_t        boot_state;

    /* Initialize the PSA crypto library*/
    status = val->crypto_function(VAL_CRYPTO_INIT);
    TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(1));

    status = val->get_boot_flag(&boot_state);
    TEST_ASSERT_EQUAL(status, VAL_STATUS_SUCCESS, TEST_CHECKPOINT_NUM(2));

    if (boot_state == BOOT_NOT_EXPECTED)
    {
        /* Set the key data buffer to the input base on algorithm */
        for (i = 0; i < num_checks; i++)
        {
            val->print(PRINT_TEST, "[Check %d] ", g_test_count++);
            val->print(PRINT_TEST, check1[i].test_desc, 0);

            /* Initialize a key policy structure to a default that forbids all
             * usage of the key
             */
            val->crypto_function(VAL_CRYPTO_KEY_POLICY_INIT, &policy);

            /* Setting up the watchdog timer for each check */
            status = val->wd_reprogram_timer(WD_CRYPTO_TIMEOUT);
            TEST_ASSERT_EQUAL(status, VAL_STATUS_SUCCESS, TEST_CHECKPOINT_NUM(3));

            if (PSA_KEY_TYPE_IS_RSA(check1[i].key_type))
             {
                if (check1[i].key_type == PSA_KEY_TYPE_RSA_KEYPAIR)
                {
                    if (check1[i].expected_bit_length == BYTES_TO_BITS(384))
                        key_data = rsa_384_keypair;
                    else if (check1[i].expected_bit_length == BYTES_TO_BITS(256))
                        key_data = rsa_256_keypair;
                    else
                        return VAL_STATUS_INVALID;
                }
                else
                {
                    if (check1[i].expected_bit_length == BYTES_TO_BITS(384))
                        key_data = rsa_384_keydata;
                    else if (check1[i].expected_bit_length == BYTES_TO_BITS(256))
                        key_data = rsa_256_keydata;
                    else
                        return VAL_STATUS_INVALID;
                }
            }
            else if (PSA_KEY_TYPE_IS_ECC(check1[i].key_type))
            {
                if (PSA_KEY_TYPE_IS_ECC_KEYPAIR(check1[i].key_type))
                    key_data = ec_keypair;
                else
                    key_data = ec_keydata;
            }
            else
                key_data = check1[i].key_data;

            /* Set the standard fields of a policy structure */
            val->crypto_function(VAL_CRYPTO_KEY_POLICY_SET_USAGE, &policy, check1[i].usage,
                                                                            check1[i].key_alg);

            /* Set the usage policy on a key slot */
            status = val->crypto_function(VAL_CRYPTO_SET_KEY_POLICY, check1[i].key_slot, &policy);
            TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(4));

            /* Change the lifetime of a key slot */
            status = val->crypto_function(VAL_CRYPTO_SET_KEY_LIFETIME, check1[i].key_slot,
                                          check1[i].lifetime);
            TEST_ASSERT_EQUAL(status, check1[i].expected_status, TEST_CHECKPOINT_NUM(5));

            /* Import the key data into the key slot */
            status = val->crypto_function(VAL_CRYPTO_IMPORT_KEY, check1[i].key_slot,
                                          check1[i].key_type, key_data, check1[i].key_length);
            TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(6));

            /* Change the lifetime of an occupied key slot */
            status = val->crypto_function(VAL_CRYPTO_SET_KEY_LIFETIME, check1[i].key_slot,
                                          check1[i].lifetime);
            TEST_ASSERT_DUAL(status, PSA_SUCCESS, PSA_ERROR_OCCUPIED_SLOT, TEST_CHECKPOINT_NUM(7));
        }

        status = val->set_boot_flag(BOOT_EXPECTED_CRYPTO);
        TEST_ASSERT_EQUAL(status, VAL_STATUS_SUCCESS, TEST_CHECKPOINT_NUM(8));
#if 0
        /*Waiting here for watchdog expiry */
        while (1);
#endif
    }
    else if (boot_state == BOOT_EXPECTED_CRYPTO)
    {
        for (i = 0; i < num_checks; i++)
        {
            val->print(PRINT_TEST, "[Check %d] ", g_test_count++);
            val->print(PRINT_TEST, check1[i].test_desc, 0);
            if (check1[i].lifetime == PSA_KEY_LIFETIME_VOLATILE)
                continue;

            /* Set the key data buffer to the input base on algorithm */
            if (PSA_KEY_TYPE_IS_RSA(check1[i].key_type))
            {
                if (check1[i].key_type == PSA_KEY_TYPE_RSA_KEYPAIR)
                {
                    if (check1[i].expected_bit_length == BYTES_TO_BITS(384))
                        key_data = rsa_384_keypair;
                    else if (check1[i].expected_bit_length == BYTES_TO_BITS(256))
                        key_data = rsa_256_keypair;
                    else
                        return VAL_STATUS_INVALID;
                }
                else
                {
                    if (check1[i].expected_bit_length == BYTES_TO_BITS(384))
                        key_data = rsa_384_keydata;
                    else if (check1[i].expected_bit_length == BYTES_TO_BITS(256))
                        key_data = rsa_256_keydata;
                    else
                        return VAL_STATUS_INVALID;
                }
            }
            else if (PSA_KEY_TYPE_IS_ECC(check1[i].key_type))
            {
                if (PSA_KEY_TYPE_IS_ECC_KEYPAIR(check1[i].key_type))
                    key_data = ec_keypair;
                else
                    key_data = ec_keydata;
            }
            else
                key_data = check1[i].key_data;

            /* Export a key in binary format */
            status = val->crypto_function(VAL_CRYPTO_EXPORT_KEY, check1[i].key_slot, data,
                                          BUFFER_SIZE, &length);
            TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(9));

            TEST_ASSERT_EQUAL(length, check1[i].expected_key_length, TEST_CHECKPOINT_NUM(10));

            /* Check if original key data matches with the exported data */
            if (PSA_KEY_TYPE_IS_UNSTRUCTURED(check1[i].key_type))
            {
                TEST_ASSERT_MEMCMP(check1[i].key_data, data, length, TEST_CHECKPOINT_NUM(11));
            }
            else if (PSA_KEY_TYPE_IS_RSA(check1[i].key_type))
            {
                TEST_ASSERT_MEMCMP(key_data, data, length, TEST_CHECKPOINT_NUM(12));
            }
            else
            {
                return VAL_STATUS_INVALID;
            }
        }
    }
    else
    {
        val->print(PRINT_ERROR, "\tInvalid boot state\n", 0);
        return VAL_STATUS_INVALID;
    }

    return VAL_STATUS_SUCCESS;
}

int32_t psa_set_key_lifetime_negative_test(security_t caller)
{
    int              num_checks = sizeof(check2)/sizeof(check2[0]);
    int32_t          i, status;
    psa_key_policy_t policy;

    /* Initialize the PSA crypto library*/
    status = val->crypto_function(VAL_CRYPTO_INIT);
    TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(1));

    for (i = 0; i < num_checks; i++)
    {
        val->print(PRINT_TEST, "[Check %d] ", g_test_count++);
        val->print(PRINT_TEST, check2[i].test_desc, 0);

        /* Initialize a key policy structure to a default that forbids all
         * usage of the key
         */
        val->crypto_function(VAL_CRYPTO_KEY_POLICY_INIT, &policy);

        /* Setting up the watchdog timer for each check */
        status = val->wd_reprogram_timer(WD_CRYPTO_TIMEOUT);
        TEST_ASSERT_EQUAL(status, VAL_STATUS_SUCCESS, TEST_CHECKPOINT_NUM(2));

        /* Change the lifetime of a key slot */
        status = val->crypto_function(VAL_CRYPTO_SET_KEY_LIFETIME, check2[i].key_slot,
                                      check2[i].lifetime);
        TEST_ASSERT_EQUAL(status, check2[i].expected_status, TEST_CHECKPOINT_NUM(3));
     }

     return VAL_STATUS_SUCCESS;
}
