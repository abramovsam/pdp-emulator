import argparse
parser = argparse.ArgumentParser()
parser.add_argument('--input', help="input file path", type=str)
parser.add_argument('--output', help="output file path", type=str)
args = parser.parse_args()

if __name__ == '__main__':
    second_number_slice = slice(8, 14)
    with open(args.input) as fin:
        num_list = [int(line[second_number_slice].strip(), 8) for line in fin.readlines()]

    bin_list_str = ['{0:016b}'.format(num) for num in num_list]
    bin_str_true_endian = ''.join([bin_str[8:] + bin_str[:8] for bin_str in bin_list_str]) + '\n'

    with open(args.output, 'w') as fout:
        fout.write(bin_str_true_endian)
