converted with main.d

.ply file holds geometry data, with bone weights and bone indices
.skel file holds the bone graph, only inverseBindMatrices are stored - is this enough to build the skeleton hierarchy ?
.anim file holds animation keyframes per node - default position of nodes ignored currently - only bone rotation is stored

guess: rotation is euler angles?
