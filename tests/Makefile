TESTS_32I := $(filter-out rv32ui-p-ma_data.hex rv32ui-p-fence_i.hex, $(wildcard rv32ui-p-*.hex))

all:

run-32ui:
	$(foreach test, $(TESTS_32I), ../tinyrv $(test) || exit;)

run: run-32ui

clean: