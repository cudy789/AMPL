#pragma once

#include <thread>
#include <bits/stdc++.h>

#include "TDCam.h"
#include "Worker.h"

class TDCamWorker: public Worker, public TDCam {
public:
    TDCamWorker() = delete;
    ~TDCamWorker() = default;
    TDCamWorker(CamParams& c_params, std::function<bool(TagArray&)> queue_tags_callback, bool show_im);

    cv::Mat GetAnnotatedIm();


protected:

    void Init() override;
    void Execute() override;
    void Finish() override;

    bool _show_im;
    std::function<bool(TagArray&)> _queue_tags_callback;

    std::binary_semaphore _annotated_im_sem{1};
    cv::Mat _annotated_im;


};