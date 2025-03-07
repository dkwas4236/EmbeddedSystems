################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
%.obj: ../%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"/opt/ti/ccs1040/ccs/tools/compiler/ti-cgt-arm_20.2.5.LTS/bin/armcl" -mv7M3 --code_state=16 -me --include_path="/home/darion/workspace_v10/Flashing_LEDS" --include_path="/home/darion/workspace_v10/Flashing_LEDS" --include_path="/opt/ti/tirtos_cc13xx_cc26xx_2_21_01_08/products/cc13xxware_2_04_03_17272" --include_path="/opt/ti/ccs1040/ccs/tools/compiler/ti-cgt-arm_20.2.5.LTS/include" -g --c99 --gcc --diag_warning=225 --diag_warning=255 --diag_wrap=off --display_error_number --gen_func_subsections=on --abi=eabi --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '


