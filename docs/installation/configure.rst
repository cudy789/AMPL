Configure & calibrate your cameras
####################################



Detect your cameras
===================




.. code-block:: bash

   v4l2-ctl --device /dev/video0 --all # list camera information, including exposure and auto exposure settings
   v4l2-ctl --device /dev/video0 --list-formats-ext # list camera resolution & framerates in all video formats


Raspberry Pi
====================

.. toctree::
   :maxdepth: 1
