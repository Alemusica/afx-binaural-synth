
// Skeleton Max/MSP external: afx.binaural~
#include "ext.h"
#include "ext_obex.h"
#include "z_dsp.h"

#include <vector>
#include <string>
#include "../engine/CpuBinauralEngine.hpp"
#include "../engine/GpuBinauralEngine.hpp"

typedef struct _afx {
    t_pxobject x_obj;
    long sources;
    long mode_gpu; // 0=cpu, 1=gpu
    double sr;
    long block;

    afx::CpuBinauralEngine* cpu;
    afx::GpuBinauralEngine* gpu;
    std::vector<afx::SourcePose> poses;
} t_afx;

void *afx_class;

void afx_assist(t_afx *x, void *b, long m, long a, char *s);
void *afx_new(t_symbol *s, long argc, t_atom *argv);
void afx_free(t_afx *x);
void afx_dsp64(t_afx *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags);
void afx_perform64(t_afx *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long sampleframes, long flags, void *userparam);
void afx_pos(t_afx *x, t_symbol*, long argc, t_atom* argv);
void afx_mode(t_afx *x, t_symbol*, long argc, t_atom* argv);

extern "C" int C74_EXPORT main(void) {
    t_class *c = class_new("afx.binaural~", (method)afx_new, (method)afx_free, (long)sizeof(t_afx), 0L, A_GIMME, 0);
    class_addmethod(c, (method)afx_assist, "assist", A_CANT, 0);
    class_addmethod(c, (method)afx_dsp64, "dsp64", A_CANT, 0);
    class_addmethod(c, (method)afx_pos, "pos", A_GIMME, 0);
    class_addmethod(c, (method)afx_mode, "mode", A_GIMME, 0);
    class_dspinit(c);
    class_register(CLASS_BOX, c);
    afx_class = c;
    return 0;
}

void afx_assist(t_afx *x, void *b, long m, long a, char *s) {
    if (m == ASSIST_INLET) snprintf(s, 256, "Sorgenti mono (x%d) + messaggi (pos/mode)", (int)x->sources);
    else snprintf(s, 256, "Out L/R");
}

void *afx_new(t_symbol *s, long argc, t_atom *argv) {
    t_afx *x = (t_afx *)object_alloc((t_class*)afx_class);
    dsp_setup((t_pxobject *)x, 1); // default 1 inlet (puoi aumentare via attr @sources)
    outlet_new((t_object *)x, "signal"); // L
    outlet_new((t_object *)x, "signal"); // R

    x->sources = 1;
    x->mode_gpu = 0;
    x->sr = sys_getsr();
    x->block = sys_getblksize();
    x->cpu = new afx::CpuBinauralEngine(16);
    x->gpu = new afx::GpuBinauralEngine(128);
    x->poses.resize(16);
    return (x);
}

void afx_free(t_afx *x) {
    dsp_free((t_pxobject *)x);
    delete x->cpu; delete x->gpu;
}

void afx_mode(t_afx *x, t_symbol*, long argc, t_atom* argv) {
    if (argc>0 && atom_gettype(&argv[0])==A_SYM) {
        t_symbol* s = atom_getsym(&argv[0]);
        if (s==gensym("gpu")) x->mode_gpu = 1;
        else if (s==gensym("cpu")) x->mode_gpu = 0;
    }
}

void afx_pos(t_afx *x, t_symbol*, long argc, t_atom* argv) {
    if (argc>=4) {
        int idx = atom_getlong(argv+0);
        afx::SourcePose p{};
        p.az_deg = atom_getfloat(argv+1);
        p.el_deg = atom_getfloat(argv+2);
        p.dist_m = atom_getfloat(argv+3);
        if (idx>=0 && idx < x->cpu->numSources()) {
            x->poses[idx] = p;
            x->cpu->setPose(idx, p);
            x->gpu->setPose(idx, p);
        }
    }
}

void afx_dsp64(t_afx *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags) {
    x->sr = samplerate;
    x->block = maxvectorsize;
    x->cpu->prepare(samplerate, (int)maxvectorsize);
    x->gpu->prepare(samplerate, (int)maxvectorsize);
    object_method(dsp64, gensym("dsp_add64"), x, (method)afx_perform64, 0, NULL);
}

void afx_perform64(t_afx *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long n, long flags, void *userparam) {
    // Adattiamo a float per il core DSP
    static thread_local std::vector<float> tmp_in, outL, outR;
    tmp_in.resize(n);
    outL.resize(n); outR.resize(n);
    const float* inPtrs[1] = { tmp_in.data() };

    // Per ora: 1 sorgente; estendi a N inlets associando ins[s]
    for (long i=0;i<n;i++) tmp_in[i] = (float)ins[0][i];

    if (x->mode_gpu) {
        x->gpu->process(inPtrs, 1, outL.data(), outR.data(), (int)n);
    } else {
        x->cpu->process(inPtrs, 1, outL.data(), outR.data(), (int)n);
    }
    for (long i=0;i<n;i++) {
        outs[0][i] = outL[i];
        outs[1][i] = outR[i];
    }
}
