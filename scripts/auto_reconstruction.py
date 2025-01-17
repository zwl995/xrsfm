import os
from argparse import ArgumentParser


def get_opts():
    parser = ArgumentParser()
    parser.add_argument('--workspace_path', type=str, required=True,
                        help='workspace_path')
    # parser.add_argument('--start_from_ios_binary',action='store_true',
    #                     help='estimate_scale') 
    parser.add_argument('--estimate_scale',action='store_true',
                        help='estimate_scale') 
    return parser.parse_args()

if __name__ == "__main__":
    args = get_opts()
 
    data_path = args.workspace_path 
 
    images_path = data_path+'images/'
    camera_path = data_path+'camera.txt'
    retrival_path = data_path+'retrival.txt'

    output_path = data_path
    output_refined_path = data_path+'refined/'
    os.system('mkdir '+output_refined_path)

    # if(args.start_from_ios_binary):
    #     ios_file = data_path+'Data.txt'
    #     os.system('./bin/unpack_collect_data '+ios_file+' '+data_path)
    os.system('./bin/run_matching '+images_path+' '+retrival_path+' sequential '+output_path)
    os.system('./bin/run_reconstruction '+output_path+' '+camera_path+' '+output_path)
    if(args.estimate_scale):
        os.system('./bin/estimate_scale '+images_path+' '+output_path+' '+output_refined_path)