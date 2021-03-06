# Script to plot pose estimation errors (3D translation & rotation) live using signal-scope
# Run script compute_transl_rot_errors.py to publish 'POSE_ERRORS' message

def makePlots():
    # translation error plot
    addPlot(yLimits=[0, 0.5])
    addSignal('POSE_ERRORS_GEORGI', msg.utime, msg.values[0])
    addSignal('POSE_ERRORS_GEORGI', msg.utime, msg.values[1])
    addSignal('POSE_ERRORS_GEORGI', msg.utime, msg.values[2])

    # rotation error plot
    addPlot(yLimits=[0, 0.5])
    addSignal('POSE_ERRORS_GEORGI', msg.utime, msg.values[3])

    setFormatOptions(pointSize=4,timeWindow=100.0,curveStyle="lines")

makePlots()
