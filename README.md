<div align="center">    
 
# UnrealROX+

[![Paper](http://img.shields.io/badge/preprint-arxiv.2104.11776-B31B1B.svg)](https://arxiv.org/abs/2104.11776)
[![Conference](https://img.shields.io/badge/IJCNN-2021-blue.svg)](https://arxiv.org/abs/2104.11776)

<img src="assets/unrealrox.png">
 
</div>
 
## Description   
UnrealROX+ is a plugin for Unreal Engine 4 that allows to easily acquire a wide variety of data from a virtual 3D environment. This data ranges from image data (RGB, depth maps, normal maps, albedo or segmentation masks) to 6D pose of objects, cameras or skeletons. Due to the high realism that UE4 achieves when rendering 3D graphics (including real-time raytracing), data generated through this tool can be used for training deep learning models. This synthetically generated data is extremely useful for these models due to its perfect precission across data, such as, pixel-perfect segmentations masks compared with its correspondent RGB image. This kind of annotations are tedious to label in real-world images, but they can be automatically obtained from graphic engines like UE4.

This tool is an improved version of [UnrealROX](https://github.com/3dperceptionlab/unrealrox), which was the original tool developed to generate [the RobotriX dataset](https://github.com/3dperceptionlab/therobotrix). In UnrealROX+ we decoupled the data-acquire subsystem, which was very tied to the workflow for genertaing RobotriX. The result is a much more flexible and easy-to-use tool that enables reasearchers to generate data for a wider amount of applications. More details can be read in the publications linked below.

## How to use   
Download the plugin for your UE4 version from the releases page. Add it to Unreal like any other plugin.
More details very soon...

## Documentation
* [Python API](docs/python-api)
* More documentation soon...

## Publication
<div align="center">

 ### [**UnrealROX+: An Improved Tool for Acquiring Synthetic Data from Virtual 3D Environments**](https://arxiv.org/abs/2104.11776)
 
 Submitted to International Joint Conference on Neural Networks (IJCNN) - 2021
 
 *Pablo Martínez-González, Sergiu Oprea, John A. Castro-Vargas, Alberto García-García, Sergio Orts-Escolano, José García-Rodríguez, and Markus Vincze*

</div>

### How to cite this work?
If you use UnrealROX+, please cite:
```
@article{Martinez2021unrealroxplus,
  author    = {Pablo Martinez{-}Gonzalez and Sergiu Oprea and John A. Castro{-}Vargas and Alberto Garcia{-}Garcia and Sergio Orts{-}Escolano and Jose Garcia{-}Rodriguez and Markus Vincze},
  title     = {UnrealROX+: An Improved Tool for Acquiring Synthetic Data from Virtual 3D Environments},
  journal   = {CoRR},
  volume    = {abs/2104.11776},
  year      = {2021}
}
```

Consider also citing the original version of [UnrealROX](https://github.com/3dperceptionlab/unrealrox), where the base system is detailed:
```
@article{Martinez2019unrealrox,
  author    = {Pablo Martinez{-}Gonzalez and Sergiu Oprea and Alberto Garcia{-}Garcia and Alvaro Jover{-}Alvarez and Sergio Orts{-}Escolano and Jose Garcia{-}Rodriguez},
  title     = {UnrealROX: An eXtremely Photorealistic Virtual Reality Environment for Robotics Simulations and Synthetic Data Generation},
  journal   = {Virtual Reality},
  issn      = "1434-9957",
  doi       = "10.1007/s10055-019-00399-5",
  url       = "https://doi.org/10.1007/s10055-019-00399-5",
  year      = {2019}
}
```

Any criticism and improvements is welcome using the issue system from this repository. For other questions, contact the corresponding authors:

- Pablo Martinez-Gonzalez [Design, UE4 Backend, Lead Programmer] ([pmartinez@dtic.ua.es](mailto:pmartinez@dtic.ua.es))
- Sergiu Oprea [Design, Grasping, Support Programmer] ([soprea@dtic.ua.es](mailto:soprea@dtic.ua.es))
- Alberto Garcia-Garcia [Design, Prototyping, Project Lead] ([agarcia@dtic.ua.es](mailto:agarcia@dtic.ua.es))
- John A. Castro-Vargas [Design, Support Programmer] ([jacastro@dtic.ua.es](mailto:jacastro@dtic.ua.es))
- Alvaro Jover-Alvarez [UE4 Expert, Support Programmer] ([ajover@dtic.ua.es](mailto:ajover@dtic.ua.es))
- Sergio Orts-Escolano [Design, Technical Advice] ([sorts@ua.es](mailto:sorts@ua.es))
- Jose Garcia-Rodriguez [Technical Advice] ([jgarcia@dtic.ua.es](mailto:jgarcia@dtic.ua.es))
