import pybullet as p
import pybullet_data
import time
import numpy as np
from scipy.spatial.transform import Rotation as R
import json
import cv2

# Camera parameters
fov = 60  # Field of view
aspect_ratio = 4.0/3.0
# Render distances
near_plane = 0.03
far_plane = 1000

# Total runtime is num_frames / framerate
num_frames = 600
framerate = 60

p_x = np.hstack((np.linspace(0, 0, int(num_frames/2)), np.linspace(0, 5, int(num_frames/2))))
p_y = np.hstack((np.linspace(-5, -2, int(num_frames/2)), np.linspace(-2, -2, int(num_frames/2))))
p_z = np.hstack((np.linspace(0.25, 0.25, int(num_frames/2)), np.linspace(0.25, 0.25, int(num_frames/2))))
p_roll = np.hstack((np.linspace(0, 0, int(num_frames/2)), np.linspace(0, 0, int(num_frames/2))))
p_pitch = np.hstack((np.linspace(0, 0, int(num_frames/2)), np.linspace(0, 0, int(num_frames/2))))
p_yaw = np.hstack((*np.sin(np.linspace(0, 2*np.pi, int(num_frames/2))), *np.sin(np.linspace(2*np.pi, 4*np.pi, int(num_frames/2)))))

positions = np.stack([p_x, p_y, p_z, p_roll, p_pitch, p_yaw], axis=1)

# Populate AprilTags from a .fmap file
def populate_apriltags(fmap_file):
    tags = []
    with open(fmap_file, "r") as f:
        fmap_data = json.load(f)

    for tag in fmap_data["fiducials"]:
        id = tag["id"]
        transform = np.array(tag["transform"]).reshape((4,4))
        translation = transform[:3,-1].reshape(-1)
        rotation = R.from_euler("xyz", (0,0,-90), degrees=True).as_matrix() @ R.from_matrix(transform[:3, :3]).as_matrix()

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
    rotation = R.from_euler('xyz', [roll, pitch, yaw], degrees=False)
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
    # Connect to PyBullet physics server
    # p.connect(p.GUI)
    p.connect(p.DIRECT)

    # Set up the physics simulation
    p.setAdditionalSearchPath(pybullet_data.getDataPath())  # Path to PyBullet data files
    # p.setPhysicsEngineParameter(enableFileCaching=0)

    p.configureDebugVisualizer(p.COV_ENABLE_RENDERING, 1)
    p.setGravity(0, 0, -9.8)

    # Load a flat plane and apriltags into the scene
    plane_id = p.loadURDF("plane.urdf")
    populate_apriltags("../fmap/crescendo_2024.fmap")

    # Setup recording
    left_filename = "sim_cam_left.mp4"
    left_out = cv2.VideoWriter(left_filename, cv2.VideoWriter_fourcc(*'mp4v'), framerate, (640,480))

    right_filename = "sim_cam_right.mp4"
    right_out = cv2.VideoWriter(right_filename, cv2.VideoWriter_fourcc(*'mp4v'), framerate, (640,480))


    with open("sim_trajectory.csv", "w") as f:

        progress_tracker=0
        time = 0
        for i, pos in enumerate(positions):

            rgb_image_left = render_camera(pos, [-0.25, 0, 0, 0, 15, 0])
            rgb_image_right = render_camera(pos, [0.25, 0, 0, 0, 15, 0])
            left_out.write(rgb_image_left)
            right_out.write(rgb_image_right)

            percent_complete = round(i/len(positions) * 100, 2)
            print("{}% complete".format(percent_complete))

            f.write(str(round(time, 9)*1.0e9) + "," + ",".join((str(x) for x in pos.tolist())) + "\n")

            time += 1.0/framerate

        left_out.release()
        right_out.release()

    print("finished")


if __name__ == "__main__":
    main()