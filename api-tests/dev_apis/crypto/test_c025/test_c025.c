/** @file
 * Copyright (c) 2019-2024, Arm Limited or its affiliates. All rights reserved.
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

#include "val_interfaces.h"
#include "val_target.h"
#include "test_c025.h"
#include "test_data.h"

const client_test_t test_c025_crypto_list[] = {
    NULL,
    psa_aead_decrypt_test,
    NULL,
};

int32_t psa_aead_decrypt_test(caller_security_t caller __UNUSED)
{
    int32_t               i, status;
    size_t                expected_plaintext_length;
    int32_t               num_checks = sizeof(check1)/sizeof(check1[0]);
    psa_key_attributes_t  attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_id_t          key;

    if (num_checks == 0)
    {
        val->print(PRINT_TEST, "No test available for the selected crypto configuration\n", 0);
        return RESULT_SKIP(VAL_STATUS_NO_TESTS);
    }

    /* Initialize the PSA crypto library*/
    status = val->crypto_function(VAL_CRYPTO_INIT);
    TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(1));

    for (i = 0; i < num_checks; i++)
    {
        val->print(PRINT_TEST, "[Check %d] ", i+1);
        val->print(PRINT_TEST, check1[i].test_desc, 0);

        /* Setting up the watchdog timer for each check */
        status = val->wd_reprogram_timer(WD_CRYPTO_TIMEOUT);
        TEST_ASSERT_EQUAL(status, VAL_STATUS_SUCCESS, TEST_CHECKPOINT_NUM(2));

        /* Setup the attributes for the key */
        val->crypto_function(VAL_CRYPTO_SET_KEY_TYPE,        &attributes, check1[i].type);
        val->crypto_function(VAL_CRYPTO_SET_KEY_USAGE_FLAGS, &attributes, check1[i].usage_flags);
        val->crypto_function(VAL_CRYPTO_SET_KEY_ALGORITHM,   &attributes, check1[i].key_alg);

        /* Import the key data into the key slot */
        status = val->crypto_function(VAL_CRYPTO_IMPORT_KEY, &attributes, check1[i].data,
                                      check1[i].data_length, &key);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(3));

        /* Process an authenticated decryption operation */
        status = val->crypto_function(VAL_CRYPTO_AEAD_DECRYPT, key, check1[i].aead_alg,
                                      check1[i].nonce, check1[i].nonce_length,
                                      check1[i].additional_data, check1[i].additional_data_length,
                                      check1[i].ciphertext, check1[i].ciphertext_length,
                                      check1[i].plaintext, check1[i].plaintext_size,
                                      &expected_plaintext_length);
        TEST_ASSERT_DUAL(status,
                         check1[i].expected_status[0],
                         check1[i].expected_status[1],
                         TEST_CHECKPOINT_NUM(4));

        if ((check1[i].expected_status[0] != PSA_SUCCESS) &&
            (check1[i].expected_status[1] != PSA_SUCCESS))
        {
            /* Destroy the key */
            status = val->crypto_function(VAL_CRYPTO_DESTROY_KEY, key);
            TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(5));

            continue;
        }

        /* Check if the length matches */
        TEST_ASSERT_EQUAL(expected_plaintext_length, check1[i].expected_plaintext_length,
                          TEST_CHECKPOINT_NUM(6));

        /* Check if the data matches */
        TEST_ASSERT_MEMCMP(check1[i].plaintext, check1[i].expected_plaintext,
                           expected_plaintext_length, TEST_CHECKPOINT_NUM(7));

        /* Destroy the key */
        status = val->crypto_function(VAL_CRYPTO_DESTROY_KEY, key);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(8));

        /* Reset the key attributes and check if psa_import_key fails */
        val->crypto_function(VAL_CRYPTO_RESET_KEY_ATTRIBUTES, &attributes);
        status = val->crypto_function(VAL_CRYPTO_IMPORT_KEY, &attributes, check1[i].data,
                                      check1[i].data_length, &key);
        TEST_ASSERT_EQUAL(status, PSA_ERROR_NOT_SUPPORTED, TEST_CHECKPOINT_NUM(9));

        /* Process an authenticated decryption operation on a destroyed key handle */
        status = val->crypto_function(VAL_CRYPTO_AEAD_DECRYPT, key, check1[i].aead_alg,
                                      check1[i].nonce, check1[i].nonce_length,
                                      check1[i].additional_data, check1[i].additional_data_length,
                                      check1[i].ciphertext, check1[i].ciphertext_length,
                                      check1[i].plaintext, check1[i].plaintext_size,
                                      &expected_plaintext_length);
        TEST_ASSERT_EQUAL(status, PSA_ERROR_INVALID_HANDLE, TEST_CHECKPOINT_NUM(10));
    }

    return VAL_STATUS_SUCCESS;
}
