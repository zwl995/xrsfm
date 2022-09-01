 
# Tutorial 

## Structure-from-Motion

Structure-from-motion (SfM) recover the 3D scene structure from images captrued from different viewpoints.
The input is a group of images, and the output is the point cloud of the scene observed by the images. 
Generally, the SfM system can be divided into two stages, matching stage and reconstruction stage.
In the matching stage, the feature matching algorithm is used to find the pixel correlation of the image;
In the reconstruction stage, the camera pose and 3D points are estimated through two-dimensional observation.

Reconstruction with monocular images will lose the real scale, XRSfM provides the function of estimating scale.
The scale estimation function relies on the artificial markers placed in advance in the scene.
The 3D structure with real scale can support ARDemo and other AR localization applications.

### 1.Feature extraction and matching
Input:images, retrieval file, matching strategy

Output:feature extraction result, feature matching result

Run matching stage with the following command line

```
./bin/run_matching images_path retrival_path matching_type output_path
```

"retrival_path" indicates the path of the retrieval file, which stores the results of image retrieval.
Currently, the image retrieval function is supported in [XRLocalization](https://github.com/openxrlab/xrlocalization/tree/main/docs/en/tutorials/generate_image_pairs.md).
You can also generate the file through other retrieval methods. For specific file format, please refer to [introduction.md](./introduction.md).

"matching_type" indicates the matching strategy.
Currently, XRSfM supports sequential matching("sequential"), retrieval-based matching("retrival"), covisibility-based matching("covisibility").
The covisibility-based matching is a implementation of "Efficient Covisibility-Based Image Matching for Large-Scale SfM".



### 2.Scene reconstruction
Input:feature extraction result, feature matching result, camera intrinsic file



Output: reconsturction result

Run sequential data with the following command line
```
./bin/run_reconstruction bin_path camera_path output_path init_id1 init_id2
```

"bin_path" is a folder path containing binaries of features and matching results which can be generated by Step 1. 

"camera_path" is the path of 'camera.txt'. 'camera.txt' stores the infomation of camera intrinsic parameters.

"init_id1" and "init_id2" indicates two frames for the initialization of SfM.
In general, these two frames can be set to 0 and 5.

The format of output binary files is is consistent with COLMAP, you can use colmap gui to view the reconstruction result.

### 3.Estimate scale with apriltag
Input: images, reconstruction results 

Output: reconstruction results of real scale

Run sequential data with the following command line
```
./bin/estimate_scale images_dir map_dir
```

"map_dir" store the map binary files (camera.bin, images.bin, points.bin).

This step aims to recover the true scale of the reconstruction result.
First, ensure that the input reconstruction results are correct, otherwise the whole process can not run normally.
The program will extract the apriltag from images to calculate the scale, and ensure that the apriltag of each ID is unique in the scene.

### Run your data

### Data capture

We provide [the capture application](http://doc.openxrlab.org.cn/openxrlab_docment/ARDemo/ARdemo.html#data-capturer-on-your-phone)) to capture images and acquire an accurate camera intrisic parameters at the same time.
Users can also use images from other sources. 
However, since XRSfM does not support camera self-calibration currently, users need to provide camera intrisic parameters, which can be obtained by calibration.


### Data preparation
In addition to the above image data and camera intrisic parameters, 
it is recommend to prepare the image retrieval results. . 
This image retrieval function is supported in [XRLocalization](https://github.com/openxrlab/xrlocalization/tree/main/docs/en/tutorials/generate_image_pairs.md).
Users can also directly download [test data](https://openxrlab-share.oss-cn-hongkong.aliyuncs.com/xrsfm/test_data.zip?versionId=CAEQQBiBgMCi_6mllxgiIGI2ZjM1YjE1NjBmNTRmYjc5NzZlMzZkNWY1ZTk1YWFj) to run the program.

Before running the reconstruction, you should ensure that there are the following data:
the input images (images_path),
the retrieval file (retrieval_path),
camera intrinsic file (camera_path).
Two folders are required to store the reconstruction results(results_path and refined_results_path).

### Run reconstruction
 
Run the following command line to reconstruct
```
./bin/run_matching ${images_path}$ ${retrival_path}$ sequential ${results_path}$
./bin/run_reconstruction ${results_path}$ ${camera_path}$ ${results_path}$
./bin/estimate_scale ${images_path}$ ${results_path}$ ${refined_results_path}$
```

Users can also create a workspace folder and arrange the related data in the following format.
```
workspace
-images
--image1.png
--image2.png
...
--imagen.png
-camera.txt
-retrival.txt
```
Then run the following command line to reconstruct
```
python3 ./scripts/auto_reconstruction.py --workspace_path ${workspace_path}$ --estimate_scale
``` 