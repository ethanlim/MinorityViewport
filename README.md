## Minority Viewport

### 1. Overview
This is an API server that functions has an abstraction layer for an arbitary set of depth sensors, irregardless of sensor vendors. In addition, there is a web management console in HTTP/Javascript to manage the list of API.

![Index](https://minority-viewport.s3.amazonaws.com/app/m2-sys.jpg)

**Fig 1.1 Web Management Console - Viewing**

### 2. System Requirements
 
**Minimum :**  
2 Depth Sensors - Vendor Independent  
[Minority Viewport](https://github.com/ethanlim/MinorityViewportClient "Minority Viewport Client") Depth Sensors Client  
Windows 7/Linux OS  


### 3. Setup

#### 3.1 Production Environment

1. Calibrate all the depth sensors  
	1.1 Navigate to http://LOCALHOST_URL/calibration.html  
    1.2 Select the depth sensor to calibrate  
	1.3 Choose lock and once skeleton pairs have been obtained, click calibrate

	![Calibration Module](https://minority-viewport.s3.amazonaws.com/app/calibration_module.jpg)
	**Fig 3.1 Web Management Console - Calibration**
2. View the Unified Viewport
3. 
	![Viewing](https://minority-viewport.s3.amazonaws.com/app/index-screenshot-1.jpg)
	**Fig 3.2 Web Management Console - Viewing**

#### 3.2 Development Environment

1. Clone the repo.
2. Setup [Boost](http://xpectomas.blogspot.sg/2013/10/including-boost-into-c-projects-vs2012.html "Boost Library") and [OpenCV](http://xpectomas.blogspot.sg/2014/02/installing-opencv-248-on-win-7-and.html "OpenCV")
3. Add into compile include dependencies
    C:\opencv\build\include;C:\Boost\include\boost-1_55;
4. Add into libraries include directories 
	C:\opencv\build\x86\vc11\lib;C:\Boost\lib;
5. Add into additional libraries dependencies
	opencv_core248d.lib;opencv_imgproc248d.lib;opencv_highgui248d.lib;opencv_ml248d.lib;opencv_video248d.lib;
	opencv_features2d248d.lib;opencv_calib3d248d.lib;opencv_objdetect248d.lib;opencv_contrib248d.lib;
	opencv_legacy248d.lib;opencv_flann248d.lib;

### Authors and Contributors
Lim Zhi Hao (Ethan) (@ethanlim)

### Support

Feel free to submit a ticket [here](https://github.com/ethanlim/MinorityViewport/issues)
