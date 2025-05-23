# SPDX-FileCopyrightText: Copyright (c) 2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
# SPDX-License-Identifier: Apache-2.0
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# Add testing
if(BUILD_TESTING)

  # Copy the data to the binary directory to testing
  add_custom_target(endo_ref_data_labels ALL
  COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/endo_ref_data_labels.csv" ${CMAKE_CURRENT_BINARY_DIR}
  DEPENDS "endo_ref_data_labels.csv"
  BYPRODUCTS "endo_ref_data_labels.csv"
  )

  # To get the environment path
  find_package(holoscan 0.6 REQUIRED CONFIG PATHS "/opt/nvidia/holoscan" "/workspace/holoscan-sdk/install")

  set(RECORDING_DIR ${CMAKE_CURRENT_BINARY_DIR}/recording_output)
  set(SOURCE_VIDEO_BASENAME python_multiai_endoscopy_output)
  set(VALIDATION_FRAMES_DIR ${CMAKE_SOURCE_DIR}/applications/multiai_endoscopy/testing/)

  file(MAKE_DIRECTORY ${RECORDING_DIR})

  # Configure the yaml file for testing
  file(READ "${CMAKE_CURRENT_SOURCE_DIR}/multi_ai.yaml" CONFIG_FILE)
  string(REGEX REPLACE "source:[^\n]*" "source: replayer" CONFIG_FILE ${CONFIG_FILE})
  string(REPLACE "count: 0" "count: 10" CONFIG_FILE ${CONFIG_FILE})
  file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/multi_ai_test.yaml ${CONFIG_FILE})

  # Patch the current example to enable recording the rendering window
  add_custom_command(OUTPUT multi_ai_test.py
  COMMAND patch -u -o multi_ai_test.py ${CMAKE_CURRENT_SOURCE_DIR}/multi_ai.py
      ${CMAKE_SOURCE_DIR}/applications/multiai_endoscopy/testing/python_multiai_endoscopy.patch
  COMMAND sed -i "s#RECORDING_DIR#${RECORDING_DIR}#g" multi_ai_test.py
  COMMAND sed -i "s#SOURCE_VIDEO_BASENAME#${SOURCE_VIDEO_BASENAME}#g" multi_ai_test.py
  )

  add_custom_target(multi_ai_test ALL
    DEPENDS "multi_ai_test.py"
  )

  # Add test
  add_test(NAME multiai_endoscopy_python_test
  COMMAND python3 ${CMAKE_CURRENT_BINARY_DIR}/multi_ai_test.py
          --config ${CMAKE_CURRENT_BINARY_DIR}/multi_ai_test.yaml
          --data ${HOLOHUB_DATA_DIR}/
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

  set_property(TEST multiai_endoscopy_python_test PROPERTY ENVIRONMENT
      "PYTHONPATH=${GXF_LIB_DIR}/../python/lib:${CMAKE_BINARY_DIR}/python/lib")

  set_tests_properties(multiai_endoscopy_python_test PROPERTIES
       PASS_REGULAR_EXPRESSION "Reach end of file or playback count reaches to the limit. Stop ticking.;"
       FAIL_REGULAR_EXPRESSION "[^a-z]Error;ERROR;Failed"
       TIMEOUT 1200  # Long engine generation (~1000s) observed on iGPU
  )

  # Add a test to check the validity of the frames
  add_test(NAME multiai_endoscopy_python_render_test
    COMMAND python3 ${CMAKE_SOURCE_DIR}/utilities/video_validation.py
    --source_video_dir ${RECORDING_DIR}
    --source_video_basename ${SOURCE_VIDEO_BASENAME}
    --output_dir ${RECORDING_DIR}
    --validation_frames_dir ${VALIDATION_FRAMES_DIR}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  )

  set_tests_properties(multiai_endoscopy_python_render_test PROPERTIES
    DEPENDS multiai_endoscopy_python_test
    PASS_REGULAR_EXPRESSION "Valid video output!"
  )

endif()
