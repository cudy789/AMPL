1. Add a MAPLE config yaml to sim_tests/maple_configs and a simulation generation config yaml to sim_tests/sim_configs.
2. Run the generate-sim-videos.sh script from the root MAPLE directory
3. Add generated .mp4 & .csv artifacts to your commit
4. Push artifacts to GitHub
5. The integration test script will automatically detect new tests in the sim_configs & maple_configs folder and will 
pull videos from the sim_output folder. 