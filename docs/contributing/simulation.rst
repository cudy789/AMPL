Generating & evaluating simulation videos
############################################

Simulation videos are generated using the python script ``tools/pybullet_video_gen.py``. The bash script ``test/integration/generate-sim-videos.sh``
is setup to generate a batch of simulation videos at once using configuration files to define the trajectory and camera setup
for the simulation.

Option 1. Modify the configuration files
===========================================

Change the waypoints in the ``test/integration/sim_tests/sim_configs/2025-triple-cam-pathplanner.yml`` file. You can add,
remove, or modify points. The point format is the following

``[time (seconds), x, y, z, roll, pitch, yaw]``

The first point must start at time 0.


Option 2. Create new configuration files
===========================================

To generate a new simulation scenario, you need to create two files:

1. A new .yml file ``test/integration/sim_tests/maple_configs/new-sim-config.yml``

   This file contains the MAPLE configuration parameters to be used when the video files are fed into MAPLE.

2. A new .yml file ``test/integration/sim_tests/sim_configs/new-sim-config.yml``

   This file contains the camera names, camera locations, and the trajectory for the simulation generation.

Generate the videos
===========================

.. code:: bash

   test/integration/generate-sim-videos.sh


Run MAPLE on the videos
=============================

.. code:: bash

   test/integration/ci-integration-tests.sh

Go to `localhost:8080 <localhost:8080>`_ to view MAPLE running on the simulated video.

Use ctrl-c to kill the program. If it doesn't exit gracefully, open a new terminal and run ``docker kill maple-integration``

.. toctree::
   :hidden: