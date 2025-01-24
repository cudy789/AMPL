import pybullet as p
import pybullet_data
import os
import numpy as np
from scipy.spatial.transform import Rotation as R
import json, csv
import cv2
import argparse

# Camera parameters
fov = 60  # Field of view
aspect_ratio = 4.0/3.0
# Render distances
near_plane = 0.03
far_plane = 30

framerate = 60

def gen_positions_from_waypoints(waypoints_file):
    with open(waypoints_file, "r") as f:
        lines = [l for l in csv.reader(f)]

    p_x = np.array([])
    p_y = np.array([])
    p_z = np.array([])
    p_roll = np.array([])
    p_pitch = np.array([])
    p_yaw = np.array([])

    if float(lines[0][0]) != 0.0:
        print("error, first waypoint must start at 0.0 seconds")
        exit(-1)


    for i in range(len(lines)):
        if i == 0:
            continue
        n_frames = int((float(lines[i][0]) - float(lines[i-1][0])) * framerate)

        p_x = np.hstack((p_x, np.linspace(float(lines[i-1][1]), float(lines[i][1]), n_frames)))
        p_y = np.hstack((p_y, np.linspace(float(lines[i-1][2]), float(lines[i][2]), n_frames)))
        p_z = np.hstack((p_z, np.linspace(float(lines[i-1][3]), float(lines[i][3]), n_frames)))
        p_roll = np.hstack((p_roll, np.linspace(float(lines[i-1][4]), float(lines[i][4]), n_frames)))
        p_pitch = np.hstack((p_pitch, np.linspace(float(lines[i-1][5]), float(lines[i][5]), n_frames)))
        p_yaw = np.hstack((p_yaw, np.linspace(float(lines[i-1][6]), float(lines[i][6]), n_frames)))

    positions = np.stack([p_x, p_y, p_z, p_roll, p_pitch, p_yaw], axis=1)

    return positions

# Populate AprilTags from a .fmap file
def populate_apriltags(fmap_file):
    tags = []
    with open(fmap_file, "r") as f:
        fmap_data = json.load(f)

    for tag in fmap_data["fiducials"]:
        id = tag["id"]
        transform = np.array(tag["transform"]).reshape((4,4))
        translation = transform[:3,-1].reshape(-1)
        rotation = R.from_matrix(transform[:3, :3]).as_matrix()

        b_tag = p.loadURDF("./at_objs/at.urdf", translation, R.from_matrix(rotation).as_quat())

        if (id < 10):
            id_str = "0000{}".format(id)
        else:
            id_str = "000{}".format(id)

        texture_id = p.loadTexture("./at_objs/apriltag-imgs/tag36h11/tag36_11_{}-big-cleaned.png".format(id_str))
        p.changeVisualShape(b_tag, -1, textureUniqueId=texture_id)


# Function to render the RGB image from the synthetic camera
def render_camera(position, cam_extrinsic):
    # Extract position and orientation components
    x, y, z, roll, pitch, yaw = position

    # Compute the rotation matrix and camera direction
    rotation = R.from_euler('yxz', [roll, pitch, yaw], degrees=True)
    rotation_matrix = rotation.as_matrix()

    extrinsic_rotation = R.from_euler('yxz', [*cam_extrinsic[3:]], degrees=True)
    rotation_matrix = rotation_matrix @ extrinsic_rotation.as_matrix()


    # Camera up vector and forward vector in local frame
    camera_forward_local = np.array([0, 1, 0])  # Y-axis is forward
    camera_up_local = np.array([0, 0, 1])       # Z-axis is up

    # Transform to global frame
    camera_forward_global = rotation_matrix @ camera_forward_local
    camera_up_global = rotation_matrix @ camera_up_local

    camera_position = np.array([x, y, z])
    camera_position += cam_extrinsic[:3]

    # Calculate camera target and up vector

    camera_target = camera_position + camera_forward_global
    camera_up = camera_up_global

    view_matrix = p.computeViewMatrix(camera_position, camera_target.tolist(), camera_up.tolist())  # Camera up vector
    projection_matrix = p.computeProjectionMatrixFOV(fov, aspect_ratio, near_plane, far_plane)

    width, height, rgb_img, _, _ = p.getCameraImage(
        width=640,
        height=480,
        viewMatrix=view_matrix,
        projectionMatrix=projection_matrix,
    )
    return np.reshape(rgb_img, (height, width, 4))[:, :, :3]  # Return the RGB part of the image


def main():

    parser = argparse.ArgumentParser()
    parser.add_argument("--headless", help="run in headless mode", action="store_true")
    parser.add_argument("--user-control", help="user moves the camera around scene instead of following predetermined trajectory", action="store_true")
    parser.add_argument("--waypoints", help="specify the .csv file with waypoints to generate the trajectory for the camera to follow", default="sim_waypoint_input.csv")
    parser.add_argument("--fmap", help="specify the .fmap file with apriltag locations and orientations", default="../fmap/field.fmap")

    args = parser.parse_args()

    # Connect to PyBullet physics server
    if (args.headless):
        p.connect(p.DIRECT)
    else:
        p.connect(p.GUI)

    # Set up the physics simulation
    p.setAdditionalSearchPath(pybullet_data.getDataPath())  # Path to PyBullet data files
    # p.setPhysicsEngineParameter(enableFileCaching=0)

    p.configureDebugVisualizer(p.COV_ENABLE_RENDERING, 1)
    p.setGravity(0, 0, -9.8)

    # Load a flat plane and apriltags into the scene
    plane_id = p.loadURDF("plane.urdf")
    populate_apriltags(args.fmap)

    # Setup recording
    left_filename = "." + str(args.waypoints[:-4]) + "_left.mp4"
    left_out = cv2.VideoWriter(left_filename, cv2.VideoWriter_fourcc(*'mp4v'), framerate, (640,480))

    right_filename = "." + str(args.waypoints[:-4]) + "_right.mp4"
    right_out = cv2.VideoWriter(right_filename, cv2.VideoWriter_fourcc(*'mp4v'), framerate, (640,480))


    with open(str(args.waypoints[:-4])+"_gt.csv", "w") as f:

        progress_tracker=0
        time = 0
        if (args.user_control):
            pos = [0,0,0,0,0,0]
            rgb_image_left = render_camera(pos, [-0.25, 0, 0.5, 0, 15, 0])
            rgb_image_right = render_camera(pos, [0.25, 0, 0.5, 0, 15, 0])
        else:
            positions = gen_positions_from_waypoints(args.waypoints)
            for i, pos in enumerate(positions):

                rgb_image_left = render_camera(pos, [0, 0, 0, 0, 0, 0])
                rgb_image_right = render_camera(pos, [0, 0, 0, 0, 30, 0])
                left_out.write(rgb_image_left)
                right_out.write(rgb_image_right)

                percent_complete = round(i/len(positions) * 100, 2)
                if (percent_complete >= progress_tracker):
                    progress_tracker += 10
                    print("{}% complete".format(percent_complete))

                f.write(str(round(time, 9)*1.0e9) + "," + ",".join((str(x) for x in pos.tolist())) + "\n")

                time += 1.0/framerate

            left_out.release()
            right_out.release()
            os.rename(left_filename, left_filename[1:])
            os.rename(right_filename, right_filename[1:])



    print("100.0% complete\nfinished")


if __name__ == "__main__":
    main()