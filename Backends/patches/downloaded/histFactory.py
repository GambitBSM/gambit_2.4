import os
import yoda
import re
import rivet
import sys
import numpy as np
from contur import TestingFunctions as ctr
from contur.TestingFunctions import buildCov as cov
import contur.Utils as util
from conturBucket import conturBucket

import contur

import logging

ANALYSIS = re.compile(r'([A-Z0-9]+_\d{4}_[IS]\d{5,8}[^/]*)')
ANALYSISPATTERN = re.compile(r'([A-Z0-9]+_\d{4}_[IS]\d{5,8})')
# free this up for the test analyses
ANALYSISHISTO= re.compile('[A-Z0-9]+_\d{4}_[IS]\d{4,8}[^\/]\S*')



#TODO replace the regex matching with the rivet libary option rivet.AOPath(path).basepathparts(keepref=False)

from tqdm import tqdm

global REFLOAD

REFLOAD = False
refObj = {}
thyObj = {}
refCorr= {}
refUncorr={}
refErrors={}
theoryCorr={}
theoryUncorr={}
theoryErrors={}

#A one off to hold objects that have been scaled in an unscaled form for plotting
plotObj={}
plotThObj={}

def init_ref(aopath=[]):
    """Function to load all reference data and theory *.yoda data

        :arg aopath:
            List of rivet.AOPath ID's to load, default empty list loads everything in $RIVET_REF_PATH
        :type aopath: ``list`` of ``string``

    """
    refFiles = []
    #contur.gridMode=False
    #if aopath and contur.fastMode and not contur.gridMode:
        #if the input list isn't set it's hard to code up logic to veto analyses etc...
        # this bit only works in fast mode. 
    if contur.onlyAnalyses:
        temp=[]
        for only in contur.onlyAnalyses:#[ANALYSIS.search(x).group() for x in contur.onlyAnalyses if ANALYSIS.search(x)]:
            temp.extend([x for x in aopath if only in rivet.stripOptions(x) ])

        aopath=temp
    if contur.vetoAnalyses:
        temp=[]
        for veto in contur.vetoAnalyses: #[ANALYSIS.search(x).group() for x in contur.vetoAnalyses if ANALYSIS.search(x)]:
            temp.extend([x for x in aopath if veto not in rivet.stripOptions(x)])
        aopath=temp
        #check if the list is now null to exit early
    if not aopath:
        contur.conturLog.critical("Choice of veto or only analyses left list of signal yoda empty, try different veto arguments")
        sys.exit(1)

    contur.conturLog.info("Loading reference and theory data from all yoda files in $RIVET_DATA_PATH matching paths in input yoda")
    rivet_data_dirs = rivet.getAnalysisRefPaths()
    rivet_plot_dirs = rivet.getAnalysisPlotPaths()
    global plotInfo
    plotInfo = rivet.mkStdPlotParser(rivet_plot_dirs)
    for dirs in rivet_data_dirs:
        import glob
        refFiles.append(glob.glob(os.path.join(dirs, '*.yoda')))
    for fileList in refFiles:
        for f in tqdm(fileList, desc="Processing reference/theory YODAs"):
            #if contur.fastMode and aopath:
            if aopath:
                #An even more extreme version of the speedup, only read the file in the first place if it matches the analysis string
                #where the acceptable strings are taken from the signal yoda
                match = ANALYSISPATTERN.search(f)
                if match:
                    if not any([match.group() in x for x in set([ANALYSISPATTERN.search(x).group() for x in aopath if ANALYSISPATTERN.search(x)])]):
                        continue
                else:
                    continue

            aos = yoda.read(f)
            for path, ao in aos.iteritems():
                #since Rivet3 we now book RAW histos, get rid of these to speed up this loop
                if not ( rivet.isRefPath(path) or rivet.isTheoryPath(path) ):
                    continue
                if ao.type() =="Scatter3D":
                    continue
                if aopath: # and contur.fastMode:
                    # To save time, only load histograms that appear explictly in the signal yoda paths
                    # This breaks multi-beam grid-mode running so turned off by default. 
                    # Could still save some time by having a list of experiments to search for.
                    match=ANALYSISHISTO.search(rivet.stripOptions(path))                    
                    if match:
                        if not any([match.group() in x for x in [rivet.stripOptions(x) for x in aopath]]):
                            continue
                    else:
                        continue
                #otherwise everything is fair game, but this is slower
                if ao.type() != "Scatter2D":
                    ao = yoda.mkScatter(ao)
                if ao.type() == "Scatter1D":
                    ao = util.mkScatter2D(ao)

                _isScaled, _scaleFactorData, _scaleMC = ctr.isNorm(ao.path())
                #build the covariance object to fill the dictionaries
                c = cov.covarianceBuilder(ao)
                if rivet.isRefPath(path):

                    if _isScaled:
                        if not contur.gridMode:
                            plotObj[path]=ao.clone()
                        # NB this scales points and errors but not the error breakdown.
                        scaleScatter2D(ao,_scaleFactorData)
                    refObj[path] = ao
                    if c.hasBreakdown and contur.buildCorr:

                        refCorr[path] = c.buildCovFromBreakdown(ignore_corrs=False)
                        # NB need this because scaleScatter doesn't scale the breakdown.
                        if _isScaled:
                            refCorr[path]*= (_scaleFactorData ** 2.0)
                    # always fill the unCorr case in case we need it later
                    refUncorr[path] = c.buildCovFromErrorBar(assume_correlated=False)
                    refErrors[path] = c.getErrorBreakdown()
                    # NB don't need to scale the errors again because they were already scaled in the "scaleScatter" step.

                if rivet.isTheoryPath(path):

                    if _isScaled:
                        if not contur.gridMode:
                            plotThObj[path] = ao.clone()
                        # NB this scales points and errors but not the error breakdown.
                        scaleScatter2D(ao, _scaleFactorData)
                    thyObj[path] = ao

                    if c.hasBreakdown and contur.buildCorr:
                        if contur.useTheoryCorr:
                            theoryCorr[path]=c.buildCovFromBreakdown(ignore_corrs=False)
                            # NB need this because scaleScatter doesn't scale the breakdown.
                            if _isScaled:
                                theoryCorr[path] *= (_scaleFactorData ** 2.0)
                        # always fill the unCorr case in case we need it later
                        theoryUncorr[path] = c.buildCovFromBreakdown(ignore_corrs=True)
 
                    else:
                        if contur.useTheoryCorr:
                            theoryCorr[path]=c.buildCovFromErrorBar(assume_correlated=True)
                        theoryUncorr[path]=c.buildCovFromErrorBar(assume_correlated=False)

                    # NB don't need to scale the errors again because they were already scaled in the "scaleScatter" step.
                    theoryErrors[path]=c.getErrorBreakdown()



    contur.conturLog.info("Done loading static data")
    #global REFLOAD
    REFLOAD = True

def scaleScatter2D(ao,sf):
# NB: would be neater if this scaled the error breakdown too, but too slow.
    for i in range(0, len(ao.points())):
        ao.points()[i].setY(ao.points()[i].y() * sf)
        ao.points()[i].setYErrs(map(lambda x: x * sf, ao.points()[i].yErrs()))


class histFactory(object):
    """Processes and decorates :class:`YODA.AnalysisObjects` to a testable format, filling a candidate conturBucket by default

        :param anaObj:
            YODA aos to dress
        :type anaObj: :class:`YODA.AnalysisObject`
        :param xSec:
            _XSEC scatter recording generator cross section in YODA file (*contained in all Rivet run outputs*)
        :type xSec: :class:`YODA.Scatter1D`
        :param nEv:
            _EVTCOUNT scatter recording total generated events in YODA file (*contained in all Rivet run outputs*)
        :type nEv: :class:`YODA.Scatter1D`

        :Built members:
            * *conturBucket* (:class:`contur.conturBucket`) --
              Automatically filled bucket containing statistical test pertaining to this histogram
            * Various plotting members

        :Keyword Arguments:
            * *aopath* (``list``) --
              List of rivet.AOPath ID's to load, default empty list loads everything in $RIVET_REF_PATH
            * *GridMode* (``bool``) --
              True if running in gridMode, causes less plot objects to be saved in memory

    """

    def __init__(self, anaObj, xSec, nEv, GridMode=False,aopaths=None):

        # Construct with an input yoda aos and a scatter1D for the cross section and nEv
        self.signal = anaObj
        self.xsec = xSec
        self.nev = nEv
        self.aopaths=aopaths

        self._ref = None

        self._weight = rivet.extractWeightName(self.signal.path())
        if self._weight != contur.weight:
            return

        self.signal.setPath(rivet.stripWeightName(self.signal.path()))


        # Initialize the public members we always want to access
        self._CLs=None
        self._gridMode=GridMode
        self._IDstring=''
        self._has1Dhisto = None
        self._isRatio = ctr.isRatio(anaObj.path())
        self._background = None
        self._stack = yoda.Scatter2D
        self._refplot = None
        self._sigplot = None
        self._bgplot = None
        self._thyplot = None
        self._lumi = 1
        self._isScaled = False
        self._scaleFactorData = 1
        self._scaleFactorSig = 1
        self._conturPoints = []
        self._scaleMC = 1.0
        self._maxcl = -1
        self._maxbin = -1
        self._gotTh = False
        self._cov = None
        self._uncov = None
        self._nuisErrs = None
        self._bucket = None
        self._thCov = None
        self._thUncov = None
        self._thErrs = None

        # self._covScaled= None
        # self._uncovScaled=None

        # Call the internal functions on initialization
        # to fill the above members with what we want, these should all be private
        if not self.__getData():
            return
        self.__getThy()
        if contur.theoryOnly and not self._gotTh:
            return
        self.__getAux()
        self.__getisScaled()
            
        # Determine the type of object we have, and build a 2D scatter from it if it is not one already
        # Also recalculate scalefactor, if appropriate
        if self.signal.type() == 'Histo1D' or self.signal.type() == 'Profile1D' or self.signal.type() == 'Counter':

            self._has1Dhisto = True

            if self._isScaled:
                # if the plot is area normalised (ie scaled), work out the factor from number of events and generator xs
                # (this is just the integrated cross section associated with the plot)
                try:
                    self._scaleFactorSig = (
                                               float(self.xsec.points()[0].x())) * float(self.signal.numEntries()) / float(
                        self.nev.numEntries())

                except Exception as e:
                    print e 
                    contur.conturLog.warning("missing info for scalefactor calc")


            self.signal = yoda.mkScatter(self.signal)
            # Make sure it is actually a Scatter2D - mkScatter makes Scatter1D from counter.   
            if self.signal.type() == 'Scatter1D':
                self.signal = util.mkScatter2D(self.signal)

        if not GridMode:
            # Public member function to build plots needed for direct histogram visualisation
            # avoid calling YODA.clone() unless we have to
            # Must be called before scaling.  
            contur.conturLog.info("Making raw Rivet plots for visualisation")
            if self._ref:
                self.doPlot()
            else:
                contur.conturLog.info("No reference data found for histo: %s" % self.signal.path())

        if self._ref:
            # don't scale histograms that came in as 2D scatters
            if self._has1Dhisto and self._isScaled:
                self.__doScale()
            self.__fillBucket()


    def __getisScaled(self):
        """Internal function to look up Scaling attributes from the contur database, defined in :mod:`contur.TestingFunctions`

        :Built members:
            * *isScaled* (``bool``) --
              True if some scaling has been applied to this histogram
            * *scaleFactorData* (``float``) --
              Factor to scale the ref data by (n count) to undo the normalisation

        """
        self._isScaled, self._scaleFactorData, self._scaleMC = ctr.isNorm(self.signal.path())


    def __getData(self):
        """Internal function to look up the refdata, triggers a call to :func:`contur.histFactory.init_ref`, global REFLOAD flag to check if this has already been run

        :Built members:
            * *ref* (:class:YODA.Scatter2D) --
              Reference scatter plot matching path from input signal aos
            * *cov* (:class:`numpy.array`) --
              Built covariance matrix from ref annotations
            * *uncov* (:class:`numpy.array`) --
              Built covariance matrix from ref annotations assuming all uncertainty is fully uncorrelated

        """

        try:
            self._ref = refObj["/REF" + rivet.stripOptions(self.signal.path())]
        except:
            p = re.compile(rivet.stripOptions(self.signal.path()))
            if filter(p.match, refObj.keys()):
                # we should use the regex match + ref but to fast lets use the old iteration
                for path, ao in refObj.iteritems():
                    if self.signal.path() in path and rivet.isRefPath(path):
                        self._ref = ao.clone()

        if self._ref is None: return False

        try:
            self._cov = refCorr["/REF" + rivet.stripOptions(self.signal.path())]
            self._nuisErrs = refErrors["/REF" + rivet.stripOptions(self.signal.path())]
            contur.conturLog.info("Using correlation information for %s" % self.signal.path())
        except:
            contur.conturLog.info("No correlation information for %s" % self.signal.path())
            self._cov = None
            self._nuisErrs = None
        try:
            self._uncov = refUncorr["/REF" + rivet.stripOptions(self.signal.path())]
        except:
            self._uncov = None
            return False
        return True


    def __getThy(self):
        """Internal function to look up the theory data, triggers a call to :func:`contur.histFactory.init_ref`, global REFLOAD flag to check if this has already been run

        :Built members:
            * *thyplot* (:class:YODA.Scatter2D) --
              Theory scatter plot matching path from input signal aos
            * *thcov* (:class:`numpy.array`) --
              Built covariance matrix from thy annotations
            * *thuncov* (:class:`numpy.array`) --
              Built covariance matrix from thy annotations assuming all uncertainty is fully uncorrelated
            * *background* (:class:YODA.Scatter2D) --
              Background model, either built from the ref or thy if found and want to use

        """
        # if not REFLOAD:
        #     init_ref(self.aopaths)

        # find whether theory is always required for this histogram
        self._theoryComp = ctr.theoryComp(self.signal.path())

        try:
            self._thyplot = thyObj["/THY" + rivet.stripOptions(self.signal.path())]
            self._gotTh = True
            if self._theoryComp or contur.useTheory:
                self._background = self._thyplot.clone()
                try:
                    self._thCov = theoryCorr["/THY" + rivet.stripOptions(self.signal.path())]
                    self._thErrs = theoryErrors["/THY" + rivet.stripOptions(self.signal.path())]
                except:
                    self._thCov = None
                    self._thErrs = None
                try:
                    self._thUncov = theoryUncorr["/THY" + rivet.stripOptions(self.signal.path())]
                except:
                    self._thUncov = None
                    #just warn if we can't build theory, it's less important...
                    contur.conturLog.warning("Could not build any theory error source for %s" % self.signal.path())

                contur.conturLog.info("Using theory for %s"%  self._thyplot.path())
        except:
            # No theory for this one
            self._gotTh = False

        if self._ref is not None and self._background is None:
            self._background = self._ref.clone()


    def doPlot(self):
        """Public member function to build yoda plot members for interactive runs"""
        #see if there are unscaled versions of the histos
        try:
            self._refplot=plotObj["/REF" + rivet.stripOptions(self.signal.path())]
        #otherwise the standard ref should be unscaled
        except:
            self._refplot=self._ref.clone()

        # and the same thought process for the background model, and for the theory (even if the
        # theory is not being used as background).
        try:
            if self._theoryComp or contur.useTheory:
                self._bgplot=plotThObj["/THY" + rivet.stripOptions(self.signal.path())]
            else:
                self._bgplot=self._refplot.clone()
            self._thyplot=plotThObj["/THY" + rivet.stripOptions(self.signal.path())]
        except:
            if not self._gotTh:
                self._bgplot=self._refplot.clone()
            else:
                self._bgplot=self._background.clone()


        # build stack for plotting, for histogrammed data
        if not self._isRatio:
            self.__buildStack()
        else:
            self._stack = self.signal.clone()
        self._sigplot = self.signal.clone()

    def __getAux(self):
        """Internal function to look up auxiliary attributes from the contur database, defined in :mod:`contur.TestingFunctions`

        :Built members:
            * *pool* (``string``) --
              String for analysis pool looked up from contur database
            * *subpool* (``string``) --
              String for analysis subpool looked up from contur database

        """
        self._lumi, self.pool, self.subpool = ctr.LumiFinder(self.signal.path())

    def __buildStack(self):
        """Private function to stack the signal for easier visualisation

        """

        if self.signal.type() != "Scatter2D":
            return False
        elif not self._bgplot:
            return False
        else:
            self._stack = self.signal.clone()
            if self._stack.numPoints() != self._bgplot.numPoints():
                contur.conturLog.info("%s : stack and backround have unequal n points. Skipping." % self._bgplot.path())
                return False

            for i in range(0, len(self._stack.points())):
                self._stack.points()[i].setY(self._stack.points()[i].y() * self._scaleFactorSig / self._scaleFactorData + \
                                          self._bgplot.points()[i].y())
                # set these to include only the MC stat errors, since that is what is used in the test
                self._stack.points()[i].setYErrs(
                    self.signal.points()[i].yErrs()[0] * self._scaleFactorSig / self._scaleFactorData,
                    self.signal.points()[i].yErrs()[1] * self._scaleFactorSig / self._scaleFactorData)


    def __doScale(self):
        """Private function to perform the normalisation of the signal
        """

        if self.signal.type() != "Scatter2D":
            return

        for i in range(0, len(self.signal.points())):
            self.signal.points()[i].setY(self.signal.points()[i].y() * self._scaleFactorSig)
            self.signal.points()[i].setYErrs(map(lambda x: x * self._scaleFactorSig, self.signal.points()[i].yErrs()))


    def __fillBucket(self):
        """Create a conturBucket, contains the observables from this histogram and their correlation plus statistical metrics

        :Built members:
            * *conturBucket* (:class:`contur.conturBucket`) --
              Automatically filled bucket containing statistical test pertaining to this histogram

        """
        if len(self._ref.points()) != len(self.signal.points()):
            contur.conturLog.error("Reference data and signal had unequal number of points so discarded from %s" % self.signal.path())
            return

        #if the signal came in as a scatter we take this to mean it was a ratio, mark as such,
        #slightly confusingly we have to flip the flag

        self._bucket = conturBucket(s=self.signal.yVals(), bg=self._background.yVals(), nobs=self._ref.yVals(),
                                    serr=[((abs(x.yErrs()[0])) + (abs(x.yErrs()[1]))) * 0.5 for x in self.signal.points()],
                                    bgerr=[x.yErrs()[0] for x in self._background.points()],
                                    nobserr=[x.yErrs()[0] for x in self._ref.points()], cov=self._cov,
                                    uncov=self._uncov,theorycov=self._thCov,theoryuncov=self._thUncov,
                                    nuisErrs=self._nuisErrs,thErrs=self._thErrs,
                                    ratio=self._isRatio,
                                    useTheory=(self._theoryComp or contur.useTheory),
                                    lumi=self._lumi,
                                    tags=rivet.stripOptions(self.signal.path()))
        self._bucket.pools = self.pool
        self._bucket.subpools = self.subpool

        #Lastly for convenience and user info get the bucket CLs and attach it to a member of histFactory
        self._CLs=self._bucket.CLs
         #except Exception, e:
         #   contur.conturLog.warning("Failed to build conturBucket for %s Error %s" % (self.signal.path, str(e)))
         # self._CLs=0

    @property
    def CLs(self):
        """CLs score derived from conturBucket

        :return: CLs ``float``

        """
        return self._CLs

    @property
    def background(self):
        """Background model, scaled if required

        :return: background :class:`YODA.Scatter2D`

        """
        return self._background

    @property
    def ref(self):
        """
        Reference data, observed numbers input to test, scaled if required

        :return: ref :class:`YODA.Scatter2D`

        """
        return self._ref

    @property
    def stack(self):
        """Stacked, unscaled Signal+background for plotting

        :return: stack :class:`YODA.Scatter2D`

        """
        return self._stack

    @property
    def sigplot(self):
        """Signal for plotting

        :return: sigplot :class:`YODA.Scatter2D`

        """
        return self._sigplot

    @property
    def refplot(self):
        """Reference data for plotting

        :return: refplot :class:`YODA.Scatter2D`

        """
        return self._refplot

    @property
    def bgplot(self):
        """Background data for plotting

        :return: bgplot :class:`YODA.Scatter2D`

        """
        return self._bgplot

    @property
    def thyplot(self):
        """Theory for plotting

        :return: thyplot :class:`YODA.Scatter2D`

        """
        return self._thyplot

    @property
    def isScaled(self):
        """Bool representing if there is additional scaling applied on top of luminosity

        :return: isScaled ``bool``

        """
        return self._isScaled

    @property
    def gotTheory(self):
        """Bool representing if a theory prediction was found for the input signal

        :return: gotTheory ``bool``

        """
        return self._gotTh

    @property
    def scaleFactorSig(self):
        """Scale factor applied to the signal histogram/scatter, derived generally from input nEv and xs

        :return: scaleFactorSig ``float``


        """
        return self._scaleFactorSig

    @property
    def scaleFactorData(self):
        """Scale factor applied to the refdata histogram/scatter

        :return: scaleFactorData ``float``


        """
        return self._scaleFactorData

    @property
    def conturBucket(self):
        """Returns the conturBucket, including statistical metrics, representing this histogram

        :return: conturBucket :class:`contur.conturBucket` -- Automatically filled bucket containing statistical test pertaining to this histogram

        """
        return self._bucket
