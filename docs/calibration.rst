Camera lens distortion and calibration
#########################################

Most camera lenses introduce visual distortions into the image as they're recording and should be corrected when you are
trying to make accurate measurements using cameras. `OpenCV's page on Camera Calibration <https://docs.opencv.org/4.x/dc/dbb/tutorial_py_calibration.html>`_
has an excellent description of what kinds of distortion are found in pinhole cameras and how to correct for it. MAPLE corrects
for radial and tangential distortions using these methods when the user provides ``dist_coeffs`` in their ``config.yml`` file.

.. note::
   If you have a camera & lens setup that has relatively low distortion, you can save CPU cycles by not providing ``dist_coeffs``
   in the configuration file and this will skip the undistort step when detecting Apriltags. You can see up to >2x speed improvement,
   which is very helpful for high FPS cameras.

How to calibrate your cameras in MAPLE
==========================================

.. note::

   If you have estimates for your ``fx``, ``fy``, ``cx``, or ``cy`` parameters, populate these values in your ``config.yml``.
   MAPLE will use these for its starting point when calculating the distortion coefficients. **Leave these parameters blank otherwise.**


1. Print out the `9x6 checkerboard calibration target <https://github.com/cudy789/MAPLE/blob/main/calibration/9x6checkerboardcalibrationpattern.png>`_
   onto a piece of paper and tape it down completely flat onto a clipboard. The exact size of the checkerboard doesn't
   matter, what's important is that the paper is completely flat.
2. Edit your ``config.yml`` file and add ``calibrate: true`` to your first camera you want to calibrate.
3. Restart MAPLE.
4. Refresh the MAPLE webUI. Your camera viewer should now say ``[CALIBRATING 0/20]`` in the top left corner.
5. Hold the calibration target close to the camera so that it takes up as much of the frame as possible. MAPLE will start
   taking still frames **every 0.5 seconds** and will overlay detections when it correctly identifies the checkerboard pattern.
   Slowly move and rotate the checkerboard in view of the camera until all 20 calibration images are acquired.

   .. image:: res/configuration/calibration.gif


6. Check the logfile to find your values for ``fx``, ``fy``, ``cx``, ``cy``, and ``dist_coeffs``. Copy and paste these
   into your ``config.yml``, then remove the ``calibrate: true`` directive.

   .. code:: bash

      [2024-12-20_14:54:46.983] INFO:
      ########################################
      CAM_REAR results
           Copy and paste the following into your config.yaml file under CAM_REAR to apply the calibration:

           fx: 523.534
           fy: 519.153
           cx: 320.269
           cy: 254.851
           dist_coeffs: [0.0272053, -0.213269, 0.00484789, 0.00450489, 0.16578]
      ########################################

########################################


7. Repeat steps 2-6 for each camera on your robot.

.. toctree::
   :maxdepth: 1
