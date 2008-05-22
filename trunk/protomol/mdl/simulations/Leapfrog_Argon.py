# USING THE NEW STRUCTURE
from Physical import *
from Forces import *
from Propagator import *
from IO import *
from ForceField import *

# PHYSICAL
phys = Physical()
io = IO()
io.readPDBPos(phys, "data/argon_280/untitled.pdb")
io.readPSF(phys, "data/argon_280/argon.psf")
io.readPAR(phys, "data/argon_280/argon.par")
phys.bc = "Periodic"
phys.cellsize = 6.5
phys.exclude = "scaled1-4"
phys.temperature = 106


# FORCES
forces = Forces()
ff = forces.makeForceField(phys)
ff.nonbondedForces("l")
ff.params['LennardJones'] = {'algorithm':'Cutoff',
                             'switching':'C1',
                             'cutoff':8.0}

# OUTPUT
io.plots = {'potentialenergy':2}
io.screen = 2

# EXECUTE
prop = Propagator(phys, forces, io)
gamma = prop.propagate("Leapfrog", steps=200, dt=20.0, forcefield=ff)
