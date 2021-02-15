#include <stdio.h>
#include "slow5_defs.h"
#include "slow5.h"
#include "press.h"

void make_blow5(void) {

    FILE *fp = fopen("test/data/exp/one_fast5/exp_1_default.blow5", "w");
    struct slow5_file *s5p = slow5_open("test/data/exp/one_fast5/exp_1_default.slow5", "r");

    char magic[] = BINARY_MAGIC_NUMBER;
    struct slow5_version version = BINARY_VERSION;
    press_method_t comp = COMPRESS_NONE;

    fwrite(magic, sizeof *magic, sizeof magic, fp);
    fwrite(&version.major, sizeof version.major, 1, fp);
    fwrite(&version.minor, sizeof version.minor, 1, fp);
    fwrite(&version.patch, sizeof version.patch, 1, fp);
    fwrite(&comp, sizeof comp, 1, fp);
    fwrite(&s5p->header->num_read_groups, sizeof s5p->header->num_read_groups, 1, fp);

    size_t padding = 64 -
        sizeof *magic * sizeof magic -
        sizeof version.major -
        sizeof version.minor -
        sizeof version.patch -
        sizeof comp -
        sizeof s5p->header->num_read_groups;
    char *zeroes = calloc(padding, 1);
    fwrite(zeroes, sizeof *zeroes, padding, fp);
    free(zeroes);

    const char *header = \
"@asic_id	3574887596\n"
"@asic_id_eeprom	0\n"
"@asic_temp	29.2145729\n"
"@auto_update	1\n"
"@auto_update_source	https://mirror.oxfordnanoportal.com/software/MinKNOW/\n"
"@bream_core_version	1.1.20.1\n"
"@bream_is_standard	1\n"
"@bream_map_version	1.1.20.1\n"
"@bream_ont_version	1.1.20.1\n"
"@bream_prod_version	1.1.20.1\n"
"@bream_rnd_version	0.1.1\n"
"@device_id	MN16450\n"
"@exp_script_name	python/recipes/nc/NC_48Hr_Sequencing_Run_FLO-MIN106_SQK-LSK108.py\n"
"@exp_script_purpose	sequencing_run\n"
"@exp_start_time	1479433093\n"
"@experiment_kit	genomic_dna\n"
"@experiment_type	customer_qc\n"
"@file_version	1\n"
"@filename	deamernanopore_20161117_fnfab43577_mn16450_sequencing_run_ma_821_r9_4_na12878_11_17_16_88738\n"
"@flow_cell_id	FAB43577\n"
"@heatsink_temp	33.9921875\n"
"@hostname	DEAMERNANOPORE\n"
"@installation_type	map\n"
"@local_firmware_file	0\n"
"@operating_system	Windows 6.2\n"
"@protocol_run_id	a4429838-103c-497f-a824-7dffa72cfd81\n"
"@protocols_version	1.1.20\n"
"@run_id	d6e473a6d513ec6bfc150c60fd4556d72f0e6d18\n"
"@sample_frequency	4000\n"
"@usb_config	1.0.11_ONT#MinION_fpga_1.0.1#ctrl#Auto\n"
"@user_filename_input	ma_821_r9.4_na12878_11_17_16\n"
"@version	1.1.20\n"
"#char*	uint32_t	double	double	double	double	uint64_t	int16_t*\n"
"#read_id	read_group	digitisation	offset	range	sampling_rate	len_raw_signal	raw_signal\n";

    uint32_t hdr_size = strlen(header);
    fwrite(&hdr_size, sizeof hdr_size, 1, fp);
    fwrite(header, sizeof *header, hdr_size, fp);

    struct slow5_rec *read = NULL;
    slow5_get_next(&read, s5p);

    slow5_rec_size_t record_size = sizeof read->read_id_len +
        read->read_id_len * sizeof *read->read_id +
        sizeof read->read_group +
        sizeof read->digitisation +
        sizeof read->offset +
        sizeof read->range +
        sizeof read->sampling_rate +
        sizeof read->len_raw_signal +
        read->len_raw_signal * sizeof *read->raw_signal;
    fwrite(&record_size, sizeof record_size, 1, fp);

    read->read_id_len = strlen(read->read_id);
    fwrite(&read->read_id_len, sizeof read->read_id_len, 1, fp);
    fwrite(read->read_id, sizeof *read->read_id, read->read_id_len, fp);
    fwrite(&read->read_group, sizeof read->read_group, 1, fp);
    fwrite(&read->digitisation, sizeof read->digitisation, 1, fp);
    fwrite(&read->offset, sizeof read->offset, 1, fp);
    fwrite(&read->range, sizeof read->range, 1, fp);
    fwrite(&read->sampling_rate, sizeof read->sampling_rate, 1, fp);
    fwrite(&read->len_raw_signal, sizeof read->len_raw_signal, 1, fp);
    fwrite(read->raw_signal, sizeof *read->raw_signal, read->len_raw_signal, fp);

    char eof[] = BINARY_EOF;

    fwrite(eof, sizeof *eof, sizeof eof, fp);

    slow5_close(s5p);
    slow5_rec_free(read);
    fclose(fp);
}

void make_gzip_blow5(void) {

    FILE *fp = fopen("test/data/exp/one_fast5/exp_1_default_gzip.blow5", "w");
    struct slow5_file *s5p = slow5_open("test/data/exp/one_fast5/exp_1_default.slow5", "r");

    char magic[] = BINARY_MAGIC_NUMBER;
    struct slow5_version version = BINARY_VERSION;
    press_method_t comp = COMPRESS_GZIP;

    fwrite(magic, sizeof *magic, sizeof magic, fp);
    fwrite(&version.major, sizeof version.major, 1, fp);
    fwrite(&version.minor, sizeof version.minor, 1, fp);
    fwrite(&version.patch, sizeof version.patch, 1, fp);
    fwrite(&comp, sizeof comp, 1, fp);
    fwrite(&s5p->header->num_read_groups, sizeof s5p->header->num_read_groups, 1, fp);

    size_t padding = 64 -
        sizeof *magic * sizeof magic -
        sizeof version.major -
        sizeof version.minor -
        sizeof version.patch -
        sizeof comp -
        sizeof s5p->header->num_read_groups;
    char *zeroes = calloc(padding, 1);
    fwrite(zeroes, sizeof *zeroes, padding, fp);
    free(zeroes);

    const char *header = \
"@asic_id	3574887596\n"
"@asic_id_eeprom	0\n"
"@asic_temp	29.2145729\n"
"@auto_update	1\n"
"@auto_update_source	https://mirror.oxfordnanoportal.com/software/MinKNOW/\n"
"@bream_core_version	1.1.20.1\n"
"@bream_is_standard	1\n"
"@bream_map_version	1.1.20.1\n"
"@bream_ont_version	1.1.20.1\n"
"@bream_prod_version	1.1.20.1\n"
"@bream_rnd_version	0.1.1\n"
"@device_id	MN16450\n"
"@exp_script_name	python/recipes/nc/NC_48Hr_Sequencing_Run_FLO-MIN106_SQK-LSK108.py\n"
"@exp_script_purpose	sequencing_run\n"
"@exp_start_time	1479433093\n"
"@experiment_kit	genomic_dna\n"
"@experiment_type	customer_qc\n"
"@file_version	1\n"
"@filename	deamernanopore_20161117_fnfab43577_mn16450_sequencing_run_ma_821_r9_4_na12878_11_17_16_88738\n"
"@flow_cell_id	FAB43577\n"
"@heatsink_temp	33.9921875\n"
"@hostname	DEAMERNANOPORE\n"
"@installation_type	map\n"
"@local_firmware_file	0\n"
"@operating_system	Windows 6.2\n"
"@protocol_run_id	a4429838-103c-497f-a824-7dffa72cfd81\n"
"@protocols_version	1.1.20\n"
"@run_id	d6e473a6d513ec6bfc150c60fd4556d72f0e6d18\n"
"@sample_frequency	4000\n"
"@usb_config	1.0.11_ONT#MinION_fpga_1.0.1#ctrl#Auto\n"
"@user_filename_input	ma_821_r9.4_na12878_11_17_16\n"
"@version	1.1.20\n"
"#char*	uint32_t	double	double	double	double	uint64_t	int16_t*\n"
"#read_id	read_group	digitisation	offset	range	sampling_rate	len_raw_signal	raw_signal\n";

    uint32_t hdr_size = strlen(header);
    fwrite(&hdr_size, sizeof hdr_size, 1, fp);
    fwrite(header, sizeof *header, hdr_size, fp);

    struct slow5_rec *read = NULL;
    slow5_get_next(&read, s5p);

    struct press *gzip = press_init(COMPRESS_GZIP);
    slow5_rec_fwrite(fp, read, FORMAT_BINARY, gzip);

    char eof[] = BINARY_EOF;
    fwrite(eof, sizeof *eof, sizeof eof, fp);

    slow5_close(s5p);
    press_free(gzip);
    slow5_rec_free(read);
    fclose(fp);
}

void make_gzip_blow5_two_rg(void) {

    FILE *fp = fopen("test/data/exp/two_rg/exp_default_gzip.blow5", "w");
    struct slow5_file *s5p = slow5_open("test/data/exp/two_rg/exp_default.slow5", "r");

    char magic[] = BINARY_MAGIC_NUMBER;
    struct slow5_version version = BINARY_VERSION;
    press_method_t comp = COMPRESS_GZIP;

    fwrite(magic, sizeof *magic, sizeof magic, fp);
    fwrite(&version.major, sizeof version.major, 1, fp);
    fwrite(&version.minor, sizeof version.minor, 1, fp);
    fwrite(&version.patch, sizeof version.patch, 1, fp);
    fwrite(&comp, sizeof comp, 1, fp);
    fwrite(&s5p->header->num_read_groups, sizeof s5p->header->num_read_groups, 1, fp);

    size_t padding = 64 -
        sizeof *magic * sizeof magic -
        sizeof version.major -
        sizeof version.minor -
        sizeof version.patch -
        sizeof comp -
        sizeof s5p->header->num_read_groups;
    char *zeroes = calloc(padding, 1);
    fwrite(zeroes, sizeof *zeroes, padding, fp);
    free(zeroes);

    const char *header = \
"@asic_id	3574887596	420170566\n"
"@asic_id_eeprom	0	2043896\n"
"@asic_temp	29.2145729	30.1846237\n"
"@auto_update	1	1\n"
"@auto_update_source	https://mirror.oxfordnanoportal.com/software/MinKNOW/	https://mirror.oxfordnanoportal.com/software/MinKNOW/\n"
"@bream_core_version	1.1.20.1	1.1.21.1\n"
"@bream_is_standard	1	1\n"
"@bream_map_version	1.1.20.1	1.1.21.1\n"
"@bream_ont_version	1.1.20.1	1.1.21.1\n"
"@bream_prod_version	1.1.20.1	1.1.21.1\n"
"@bream_rnd_version	0.1.1	0.1.1\n"
"@device_id	MN16450	MN16303\n"
"@exp_script_name	python/recipes/nc/NC_48Hr_Sequencing_Run_FLO-MIN106_SQK-LSK108.py	python/recipes/nc/NC_48Hr_Sequencing_Run_FLO-MIN106_SQK-LSK108.py\n"
"@exp_script_purpose	sequencing_run	sequencing_run\n"
"@exp_start_time	1479433093	1481657610\n"
"@experiment_kit	genomic_dna	genomic_dna\n"
"@experiment_type	customer_qc	\n"
"@file_version	1	0.6\n"
"@filename	deamernanopore_20161117_fnfab43577_mn16450_sequencing_run_ma_821_r9_4_na12878_11_17_16_88738	odw_genlab4209_20161213_fn_mn16303_sequencing_run_sample_id_32395\n"
"@flow_cell_id	FAB43577	\n"
"@heatsink_temp	33.9921875	33.9765625\n"
"@hostname	DEAMERNANOPORE	odw-genlab4209\n"
"@installation_type	map	map\n"
"@local_firmware_file	0	0\n"
"@operating_system	Windows 6.2	Windows 6.1\n"
"@protocol_run_id	a4429838-103c-497f-a824-7dffa72cfd81	3eecebf6-325f-4ac5-b896-a7588f2b2324\n"
"@protocols_version	1.1.20	1.1.21\n"
"@run_id	d6e473a6d513ec6bfc150c60fd4556d72f0e6d18	0cc960b63c07619b4bf2917507d447479a21da66\n"
"@sample_frequency	4000	4000\n"
"@usb_config	1.0.11_ONT#MinION_fpga_1.0.1#ctrl#Auto	1.0.11_ONT#MinION_fpga_1.0.1#ctrl#Auto\n"
"@user_filename_input	ma_821_r9.4_na12878_11_17_16	sample_id\n"
"@version	1.1.20	1.1.21\n"
"#char*	uint32_t	double	double	double	double	uint64_t	int16_t*\n"
"#read_id	read_group	digitisation	offset	range	sampling_rate	len_raw_signal	raw_signal\n";

    uint32_t hdr_size = strlen(header);
    fwrite(&hdr_size, sizeof hdr_size, 1, fp);
    fwrite(header, sizeof *header, hdr_size, fp);

    struct press *gzip = press_init(COMPRESS_GZIP);
    struct slow5_rec *read = NULL;

    slow5_get_next(&read, s5p);
    slow5_rec_fwrite(fp, read, FORMAT_BINARY, gzip);
    slow5_get_next(&read, s5p);
    slow5_rec_fwrite(fp, read, FORMAT_BINARY, gzip);

    char eof[] = BINARY_EOF;
    fwrite(eof, sizeof *eof, sizeof eof, fp);

    slow5_close(s5p);
    press_free(gzip);
    slow5_rec_free(read);
    fclose(fp);
}

void make_blow5_two_rg(void) {

    FILE *fp = fopen("test/data/exp/two_rg/exp_default.blow5", "w");
    struct slow5_file *s5p = slow5_open("test/data/exp/two_rg/exp_default.slow5", "r");

    char magic[] = BINARY_MAGIC_NUMBER;
    struct slow5_version version = BINARY_VERSION;
    press_method_t comp = COMPRESS_NONE;

    fwrite(magic, sizeof *magic, sizeof magic, fp);
    fwrite(&version.major, sizeof version.major, 1, fp);
    fwrite(&version.minor, sizeof version.minor, 1, fp);
    fwrite(&version.patch, sizeof version.patch, 1, fp);
    fwrite(&comp, sizeof comp, 1, fp);
    fwrite(&s5p->header->num_read_groups, sizeof s5p->header->num_read_groups, 1, fp);

    size_t padding = 64 -
        sizeof *magic * sizeof magic -
        sizeof version.major -
        sizeof version.minor -
        sizeof version.patch -
        sizeof comp -
        sizeof s5p->header->num_read_groups;
    char *zeroes = calloc(padding, 1);
    fwrite(zeroes, sizeof *zeroes, padding, fp);
    free(zeroes);

    const char *header = \
"@asic_id	3574887596	420170566\n"
"@asic_id_eeprom	0	2043896\n"
"@asic_temp	29.2145729	30.1846237\n"
"@auto_update	1	1\n"
"@auto_update_source	https://mirror.oxfordnanoportal.com/software/MinKNOW/	https://mirror.oxfordnanoportal.com/software/MinKNOW/\n"
"@bream_core_version	1.1.20.1	1.1.21.1\n"
"@bream_is_standard	1	1\n"
"@bream_map_version	1.1.20.1	1.1.21.1\n"
"@bream_ont_version	1.1.20.1	1.1.21.1\n"
"@bream_prod_version	1.1.20.1	1.1.21.1\n"
"@bream_rnd_version	0.1.1	0.1.1\n"
"@device_id	MN16450	MN16303\n"
"@exp_script_name	python/recipes/nc/NC_48Hr_Sequencing_Run_FLO-MIN106_SQK-LSK108.py	python/recipes/nc/NC_48Hr_Sequencing_Run_FLO-MIN106_SQK-LSK108.py\n"
"@exp_script_purpose	sequencing_run	sequencing_run\n"
"@exp_start_time	1479433093	1481657610\n"
"@experiment_kit	genomic_dna	genomic_dna\n"
"@experiment_type	customer_qc	\n"
"@file_version	1	0.6\n"
"@filename	deamernanopore_20161117_fnfab43577_mn16450_sequencing_run_ma_821_r9_4_na12878_11_17_16_88738	odw_genlab4209_20161213_fn_mn16303_sequencing_run_sample_id_32395\n"
"@flow_cell_id	FAB43577	\n"
"@heatsink_temp	33.9921875	33.9765625\n"
"@hostname	DEAMERNANOPORE	odw-genlab4209\n"
"@installation_type	map	map\n"
"@local_firmware_file	0	0\n"
"@operating_system	Windows 6.2	Windows 6.1\n"
"@protocol_run_id	a4429838-103c-497f-a824-7dffa72cfd81	3eecebf6-325f-4ac5-b896-a7588f2b2324\n"
"@protocols_version	1.1.20	1.1.21\n"
"@run_id	d6e473a6d513ec6bfc150c60fd4556d72f0e6d18	0cc960b63c07619b4bf2917507d447479a21da66\n"
"@sample_frequency	4000	4000\n"
"@usb_config	1.0.11_ONT#MinION_fpga_1.0.1#ctrl#Auto	1.0.11_ONT#MinION_fpga_1.0.1#ctrl#Auto\n"
"@user_filename_input	ma_821_r9.4_na12878_11_17_16	sample_id\n"
"@version	1.1.20	1.1.21\n"
"#char*	uint32_t	double	double	double	double	uint64_t	int16_t*\n"
"#read_id	read_group	digitisation	offset	range	sampling_rate	len_raw_signal	raw_signal\n";

    uint32_t hdr_size = strlen(header);
    fwrite(&hdr_size, sizeof hdr_size, 1, fp);
    fwrite(header, sizeof *header, hdr_size, fp);

    struct slow5_rec *read = NULL;
    slow5_get_next(&read, s5p);

    slow5_rec_size_t record_size = sizeof read->read_id_len +
        read->read_id_len * sizeof *read->read_id +
        sizeof read->read_group +
        sizeof read->digitisation +
        sizeof read->offset +
        sizeof read->range +
        sizeof read->sampling_rate +
        sizeof read->len_raw_signal +
        read->len_raw_signal * sizeof *read->raw_signal;
    fwrite(&record_size, sizeof record_size, 1, fp);

    read->read_id_len = strlen(read->read_id);
    fwrite(&read->read_id_len, sizeof read->read_id_len, 1, fp);
    fwrite(read->read_id, sizeof *read->read_id, read->read_id_len, fp);
    fwrite(&read->read_group, sizeof read->read_group, 1, fp);
    fwrite(&read->digitisation, sizeof read->digitisation, 1, fp);
    fwrite(&read->offset, sizeof read->offset, 1, fp);
    fwrite(&read->range, sizeof read->range, 1, fp);
    fwrite(&read->sampling_rate, sizeof read->sampling_rate, 1, fp);
    fwrite(&read->len_raw_signal, sizeof read->len_raw_signal, 1, fp);
    fwrite(read->raw_signal, sizeof *read->raw_signal, read->len_raw_signal, fp);

    slow5_get_next(&read, s5p);

    record_size = sizeof read->read_id_len +
        read->read_id_len * sizeof *read->read_id +
        sizeof read->read_group +
        sizeof read->digitisation +
        sizeof read->offset +
        sizeof read->range +
        sizeof read->sampling_rate +
        sizeof read->len_raw_signal +
        read->len_raw_signal * sizeof *read->raw_signal;
    fwrite(&record_size, sizeof record_size, 1, fp);

    read->read_id_len = strlen(read->read_id);
    fwrite(&read->read_id_len, sizeof read->read_id_len, 1, fp);
    fwrite(read->read_id, sizeof *read->read_id, read->read_id_len, fp);
    fwrite(&read->read_group, sizeof read->read_group, 1, fp);
    fwrite(&read->digitisation, sizeof read->digitisation, 1, fp);
    fwrite(&read->offset, sizeof read->offset, 1, fp);
    fwrite(&read->range, sizeof read->range, 1, fp);
    fwrite(&read->sampling_rate, sizeof read->sampling_rate, 1, fp);
    fwrite(&read->len_raw_signal, sizeof read->len_raw_signal, 1, fp);
    fwrite(read->raw_signal, sizeof *read->raw_signal, read->len_raw_signal, fp);

    char eof[] = BINARY_EOF;

    fwrite(eof, sizeof *eof, sizeof eof, fp);

    slow5_close(s5p);
    slow5_rec_free(read);
    fclose(fp);
}

int main(void) {

    make_blow5();
    make_gzip_blow5();
    make_blow5_two_rg();
    make_gzip_blow5_two_rg();

    return 0;
}