
#ifndef __UIFlowWndMovieIcons_H
#define __UIFlowWndMovieIcons_H
#include "UIFlow.h"
#include "Type.h"


extern void FlowMovie_IconDrawMaxRecTime(VControl *pCtrl);
extern void FlowMovie_IconHideMaxRecTime(VControl *pCtrl);
extern void FlowMovie_IconDrawRecTime(VControl *pCtrl);
extern void FlowMovie_IconDrawRec(VControl *pCtrl);
extern void FlowMovie_IconHideRec(VControl *pCtrl);
extern void FlowMovie_IconDrawDateTime(void);
extern void FlowMovie_IconHideDateTime(void);


extern void FlowMovie_IconDrawTopPanel(void);
extern void FlowMovie_IconHideTopPanel(void);
extern void FlowMovie_IconDrawRecURGPanel(void);
extern void FlowMovie_IconHideRecURGPanel(void);
extern void FlowMovie_DrawBrightnessBar(BOOL bDraw);

extern void FlowMovie_UpdateIcons(BOOL bShow);

#endif //__UIFlowWndMovieIcons_H
