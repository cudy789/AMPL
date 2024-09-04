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


protected:

    void Init() override;
    void Execute() override;

    bool _show_im;
    std::function<bool(TagArray&)> _queue_tags_callback;


};