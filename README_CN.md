# XRSfM


## Introduction

[English](README.md) | [简体中文] 

XRSfM 是一个开源的运动恢复结构代码仓库，它是[OpenXRLab](https://openxrlab.com/)项目的一部分.
关于XRSfM更详细的介绍放在[introduction.md](docs/en/introduction.md).

## Citation

如果你的研究过程中使用了该仓库，请考虑引用:

```bibtex
@misc{xrsfm,
    title={OpenXRLab Structure-from-Motion Toolbox and Benchmark},
    author={XRSfM Contributors},
    howpublished = {\url{https://github.com/openxrlab/xrsfm}},
    year={2022}
}
```

如果你的研究过程中使用了该仓库中的基于共视的匹配方法，请考虑引用:

```bibtex
@inproceedings{ye2020efficient,
  title={Efficient covisibility-based image matching for large-scale sfm},
  author={Ye, Zhichao and Zhang, Guofeng and Bao, Hujun},
  booktitle={IEEE International Conference on Robotics and Automation (ICRA)},
  year={2020}
}
```

## Getting Started

1.参考[installation.md](docs/zh/installation.md)进行安装编译.

2.下载提供的[测试数据](https://openxrlab-share.oss-cn-hongkong.aliyuncs.com/xrsfm/test_data.zip?versionId=CAEQQBiBgMCi_6mllxgiIGI2ZjM1YjE1NjBmNTRmYjc5NzZlMzZkNWY1ZTk1YWFj) 或者按照相同格式准备你自己的数据.

3.运行以下脚本进行重建:
```
python3 ./scripts/auto_reconstruction.py --data_path ${your_data_path}$.
```

## Build ARDemo

除了重建功能, OpenXRLab 项目还提供了定位功能。
你可以构建自己的端云定位ARDemo，更多的信息请查看[ARDemo](http://doc.openxrlab.org.cn/openxrlab_docment/ARDemo/ARdemo.html#).

## License

本项目遵从[Apache 2.0 license](LICENSE).

## Acknowledgement

XRSfM是一个开源项目，由学术界和行业的研究人员和工程师共同参与。
我们感谢所有实现其方法或添加新功能的贡献者，以及提供宝贵反馈的用户。

## Projects in OpenXRLab

- [XRPrimer](https://github.com/openxrlab/xrprimer): OpenXRLab foundational library for XR-related algorithms.
- [XRSLAM](https://github.com/openxrlab/xrslam): OpenXRLab Visual-inertial SLAM Toolbox and Benchmark.
- [XRSfM](https://github.com/openxrlab/xrsfm): OpenXRLab Structure-from-Motion Toolbox and Benchmark.
- [XRLocalization](https://github.com/openxrlab/xrlocalization): OpenXRLab Visual Localization Toolbox and Server.
- [XRMoCap](https://github.com/openxrlab/xrmocap): OpenXRLab Multi-view Motion Capture Toolbox and Benchmark.
- [XRMoGen](https://github.com/openxrlab/xrmogen): OpenXRLab Human Motion Generation Toolbox and Benchmark.
- [XRNeRF](https://github.com/openxrlab/xrnerf): OpenXRLab Neural Radiance Field (NeRF) Toolbox and Benchmark.
