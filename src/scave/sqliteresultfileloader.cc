//=========================================================================
//  SQLITERESULTFILELOADER.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Zoltan Bojthe, Andras Varga
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
   `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <utility>
#include "omnetpp/platdep/platdefs.h"
#include "common/bigdecimal.h"
#include "common/histogram.h"
#include "sqliteresultfileloader.h"

using namespace omnetpp::common;

namespace omnetpp {
namespace scave {


SqliteResultFileLoader::SqliteResultFileLoader(ResultFileManager *resultFileManagerPar) :
    IResultFileLoader(resultFileManagerPar), db(nullptr), stmt(nullptr), fileRef(nullptr)
{
}

SqliteResultFileLoader::~SqliteResultFileLoader()
{
}

inline void SqliteResultFileLoader::checkOK(int sqlite3_result)
{
    if (sqlite3_result != SQLITE_OK)
        error(sqlite3_errmsg(db));
}

inline void SqliteResultFileLoader::checkRow(int sqlite3_result)
{
    if (sqlite3_result != SQLITE_ROW)
        error(sqlite3_errmsg(db));
}

void SqliteResultFileLoader::error(const char *errmsg)
{
    throw opp_runtime_error("Cannot read SQLite result file '%s': %s", fileRef->getFileName().c_str(), errmsg);
}

void SqliteResultFileLoader::prepareStatement(const char *sql)
{
    assert(stmt == nullptr);
    checkOK(sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr));
}

void SqliteResultFileLoader::finalizeStatement()
{
    assert(stmt != nullptr);
    sqlite3_finalize(stmt);
    stmt = nullptr;
}

void SqliteResultFileLoader::loadRuns()
{
    prepareStatement("SELECT runId, runName FROM run;");

    for (int row=1; ; row++) {
        int resultCode = sqlite3_step(stmt);
        if (resultCode == SQLITE_DONE)
            break;
        checkRow(resultCode);
        sqlite3_int64 runId = sqlite3_column_int64(stmt, 0);
        std::string runName  = (const char *) sqlite3_column_text(stmt, 1);
        Run *runRef = resultFileManager->getRunByName(runName.c_str());
        if (!runRef) {
            // not yet: add it
            runRef = resultFileManager->addRun(runName);
        }
        // associate Run with this file
        if (resultFileManager->getFileRun(fileRef, runRef) != nullptr)
            error("Non-unique runId in run table");
        FileRun *fileRunRef = resultFileManager->addFileRun(fileRef, runRef);
        fileRunMap[runId] = fileRunRef;
    }
    finalizeStatement();
}

void SqliteResultFileLoader::loadRunAttrs()
{
    prepareStatement("SELECT runId, attrName, attrValue FROM runAttr;");

    for (int row=1; ; row++) {
        int resultCode = sqlite3_step (stmt);
        if (resultCode == SQLITE_DONE)
            break;
        checkRow(resultCode);
        sqlite3_int64 runId = sqlite3_column_int64(stmt, 0);
        std::string attrName = (const char *) sqlite3_column_text(stmt, 1);
        std::string attrValue = (const char *) sqlite3_column_text(stmt, 2);

        FileRun *fileRunRef = fileRunMap.at(runId);
        const StringMap& attributes = fileRunRef->runRef->getAttributes();
        StringMap::const_iterator oldPairRef = attributes.find(attrName);
        if (oldPairRef != attributes.end() && oldPairRef->second != attrValue)
            error("Value of run attribute conflicts with previously loaded value");
        fileRunRef->runRef->setAttribute(attrName, attrValue);

        // the "runNumber" attribute is also stored separately
        if (attrName == "runNumber")
            if (!parseInt(attrValue.c_str(), fileRunRef->runRef->runNumber))
                error("runNumber run attribute must be an integer");
    }
    finalizeStatement();
}

void SqliteResultFileLoader::loadRunParams()
{
    prepareStatement("SELECT runId, parName, parValue FROM runParam;");

    for (int row=1; ; row++) {
        int resultCode = sqlite3_step (stmt);
        if (resultCode == SQLITE_DONE)
            break;
        checkRow(resultCode);
        sqlite3_int64 runId = sqlite3_column_int64(stmt, 0);
        std::string parName = (const char *) sqlite3_column_text(stmt, 1);
        std::string parValue = (const char *) sqlite3_column_text(stmt, 2);

        FileRun *fileRunRef = fileRunMap.at(runId);
        fileRunRef->runRef->setModuleParam(parName, parValue);
    }
    finalizeStatement();
}

double SqliteResultFileLoader::sqlite3ColumnDouble(sqlite3_stmt *stmt, int fieldIdx)
{
    return (sqlite3_column_type(stmt, fieldIdx) != SQLITE_NULL) ? sqlite3_column_double(stmt, fieldIdx) : NAN;
}

void SqliteResultFileLoader::loadScalars()
{
    typedef std::map<sqlite3_int64,int> SqliteScalarIdToScalarIdx;
    SqliteScalarIdToScalarIdx sqliteScalarIdToScalarIdx;

    prepareStatement("SELECT scalarId, runId, moduleName, scalarName, scalarValue FROM scalar;");
    for (int row=1; ; row++) {
        int resultCode = sqlite3_step(stmt);
        if (resultCode == SQLITE_DONE)
            break;
        checkRow(resultCode);
        sqlite3_int64 scalarId = sqlite3_column_int64(stmt, 0);
        sqlite3_int64 runId = sqlite3_column_int64(stmt, 1);
        std::string moduleName = (const char *)sqlite3_column_text(stmt, 2);
        std::string scalarName = (const char *)sqlite3_column_text(stmt, 3);
        double scalarValue = sqlite3ColumnDouble(stmt,4);        // converts NULL to NaN
        int i = resultFileManager->addScalar(fileRunMap.at(runId), moduleName.c_str(), scalarName.c_str(), scalarValue, false);
        sqliteScalarIdToScalarIdx[scalarId] = i;
    }
    finalizeStatement();

    prepareStatement("SELECT scalarId, runId, attrName, attrValue FROM scalarAttr JOIN scalar USING (scalarId) ORDER BY runId, scalarId;");
    for (int row=1; ; row++) {
        int resultCode = sqlite3_step(stmt);
        if (resultCode == SQLITE_DONE)
            break;
        checkRow(resultCode);
        sqlite3_int64 scalarId = sqlite3_column_int64(stmt, 0);
        sqlite3_int64 runId = sqlite3_column_int64(stmt, 1);
        std::string attrName = (const char *)sqlite3_column_text(stmt, 2);
        std::string attrValue = (const char *)sqlite3_column_text(stmt, 3);
        SqliteScalarIdToScalarIdx::iterator it = sqliteScalarIdToScalarIdx.find(scalarId);
        if (it == sqliteScalarIdToScalarIdx.end())
            error("Invalid scalarId in scalarAttr table");
        ScalarResult& sca = fileRunMap.at(runId)->fileRef->scalarResults.at(sqliteScalarIdToScalarIdx.at(scalarId));
        sca.setAttribute(attrName, attrValue);
    }
    finalizeStatement();
}

void SqliteResultFileLoader::loadHistograms()
{
    Statistics stat;
    StringMap attrs;
    std::map<sqlite3_int64,int> sqliteStatIdToHistogramIdx;

    prepareStatement("SELECT statId, runId, moduleName, statName, statCount, "
            "statMean, statStddev, statSum, statSqrsum, statMin, statMax, "
            "statWeights, statWeightedSum, statSqrSumWeights, statWeightedSqrSum "
            "FROM statistic;");
    for (int row=1; ; row++) {
        int resultCode = sqlite3_step(stmt);
        if (resultCode == SQLITE_DONE)
            break;
        checkRow(resultCode);
        sqlite3_int64 statId = sqlite3_column_int64(stmt, 0);
        sqlite3_int64 runId = sqlite3_column_int64(stmt, 1);
        std::string moduleName = (const char *)sqlite3_column_text(stmt, 2);
        std::string statName = (const char *)sqlite3_column_text(stmt, 3);
        sqlite3_int64 statCount = sqlite3_column_int64(stmt, 4);
        double statMean = sqlite3ColumnDouble(stmt, 5); // can be computed from the others
        double statStddev = sqlite3ColumnDouble(stmt, 6); // can be computed from the others
        double statSum = sqlite3ColumnDouble(stmt, 7);
        double statSqrsum = sqlite3ColumnDouble(stmt, 8);
        double statMin = sqlite3ColumnDouble(stmt, 9);
        double statMax = sqlite3ColumnDouble(stmt, 10);
        //TODO make Scave understand weighted statistics:
        //double statWeights = sqlite3ColumnDouble(stmt, 11);
        //double statWeightedsum = sqlite3ColumnDouble(stmt, 12);
        //double statSqrSumWeights = sqlite3ColumnDouble(stmt, 13);
        //double statWeightedSqrSum = sqlite3ColumnDouble(stmt, 14);

        Statistics stat(statCount, statMin, statMax, statSum, statSqrsum);
        sqliteStatIdToHistogramIdx[statId] = resultFileManager->addHistogram(fileRunMap.at(runId), moduleName.c_str(), statName.c_str(), stat, StringMap());

        resultFileManager->addScalar(fileRunMap.at(runId), moduleName.c_str(), (statName+":count").c_str(), statCount, true);
        resultFileManager->addScalar(fileRunMap.at(runId), moduleName.c_str(), (statName+":sum").c_str(), statSum, true);
        resultFileManager->addScalar(fileRunMap.at(runId), moduleName.c_str(), (statName+":sqrsum").c_str(), statSqrsum, true);
        resultFileManager->addScalar(fileRunMap.at(runId), moduleName.c_str(), (statName+":min").c_str(), statMin, true);
        resultFileManager->addScalar(fileRunMap.at(runId), moduleName.c_str(), (statName+":max").c_str(), statMax, true);
        resultFileManager->addScalar(fileRunMap.at(runId), moduleName.c_str(), (statName+":mean").c_str(), statMean, true);
        resultFileManager->addScalar(fileRunMap.at(runId), moduleName.c_str(), (statName+":stddev").c_str(), statStddev, true);
    }
    finalizeStatement();

    prepareStatement("SELECT statId, runId, attrName, attrValue FROM statisticAttr JOIN statistic USING (statId) ORDER BY runId, statId;");
    for (int row=1; ; row++) {
        int resultCode = sqlite3_step(stmt);
        if (resultCode == SQLITE_DONE)
            break;
        checkRow(resultCode);
        sqlite3_int64 statId = sqlite3_column_int64(stmt, 0);
        sqlite3_int64 runId = sqlite3_column_int64(stmt, 1);
        std::string attrName = (const char *)sqlite3_column_text(stmt, 2);
        std::string attrValue = (const char *)sqlite3_column_text(stmt, 3);
        auto it = sqliteStatIdToHistogramIdx.find(statId);
        if (it == sqliteStatIdToHistogramIdx.end())
            error("Invalid statId in statisticAttr table");
        HistogramResult& hist = fileRunMap.at(runId)->fileRef->histogramResults.at(sqliteStatIdToHistogramIdx.at(statId));
        hist.setAttribute(attrName, attrValue);
    }
    finalizeStatement();

    prepareStatement("SELECT statId, runId, baseValue, cellValue FROM histBin JOIN statistic USING (statId) ORDER BY runId, statId;");
    for (int row=1; ; row++) {
        int resultCode = sqlite3_step(stmt);
        if (resultCode == SQLITE_DONE)
            break;
        checkRow(resultCode);
        sqlite3_int64 statId = sqlite3_column_int64(stmt, 0);
        sqlite3_int64 runId = sqlite3_column_int64(stmt, 1);
        double baseValue = sqlite3_column_double(stmt, 2);
        sqlite3_int64 cellValue = sqlite3_column_int64(stmt, 3);
        auto it = sqliteStatIdToHistogramIdx.find(statId);
        if (it == sqliteStatIdToHistogramIdx.end())
            error("Invalid statId in histBin table, or isHistogram field in the corresponding statistic table record is not set");
        HistogramResult& hist = fileRunMap.at(runId)->fileRef->histogramResults.at(sqliteStatIdToHistogramIdx.at(statId));
        hist.addBin(baseValue, cellValue);
    }
    finalizeStatement();
}

void SqliteResultFileLoader::loadVectors()
{
    std::map<sqlite3_int64,int> sqliteVectorIdToVectorIdx;

    prepareStatement(
            "SELECT vectorId, runId, moduleName, vectorName, "
            "    vectorCount, vectorMin, vectorMax, vectorSum, vectorSumSqr, "
            "    startEventNum, endEventNum, startSimtimeRaw, endSimtimeRaw, simtimeExp "
            "FROM vector LEFT JOIN run USING (runId);");

    for (int row=1; ; row++) {
        int resultCode = sqlite3_step(stmt);
        if (resultCode == SQLITE_DONE)
            break;
        checkRow(resultCode);
        sqlite3_int64 vectorId = sqlite3_column_int64(stmt, 0);
        sqlite3_int64 runId = sqlite3_column_int64(stmt, 1);
        std::string moduleName = (const char *)sqlite3_column_text(stmt, 2);
        std::string vectorName = (const char *)sqlite3_column_text(stmt, 3);
        sqlite3_int64 count = sqlite3_column_int64(stmt, 4);
        double vmin = sqlite3ColumnDouble(stmt, 5);
        double vmax = sqlite3ColumnDouble(stmt, 6);
        double vsum = sqlite3ColumnDouble(stmt, 7);
        double vsumsqr = sqlite3ColumnDouble(stmt, 8);
        sqlite3_int64 startEventNum = sqlite3_column_int64(stmt, 9);
        sqlite3_int64 endEventNum = sqlite3_column_int64(stmt, 10);
        sqlite3_int64 startSimtimeRaw = sqlite3_column_int64(stmt, 11);
        sqlite3_int64 endSimtimeRaw = sqlite3_column_int64(stmt, 12);
        int simtimeExp = sqlite3_column_int(stmt, 13);
        int i = resultFileManager->addVector(fileRunMap.at(runId), vectorId, moduleName.c_str(), vectorName.c_str(), "ETV");
        sqliteVectorIdToVectorIdx[vectorId] = i;
        VectorResult& vec = fileRunMap.at(runId)->fileRef->vectorResults.at(i);
        vec.stat = Statistics(count, vmin, vmax, vsum, vsumsqr);
        vec.startEventNum = startEventNum;
        vec.endEventNum = endEventNum;
        vec.startTime = simultime_t(startSimtimeRaw, simtimeExp);
        vec.endTime = simultime_t(endSimtimeRaw, simtimeExp);
    }
    finalizeStatement();

    prepareStatement("SELECT vectorId, runId, attrName, attrValue FROM vectorAttr JOIN vector USING (vectorId) ORDER BY runId, vectorId;");
    for (int row=1; ; row++) {
        int resultCode = sqlite3_step(stmt);
        if (resultCode == SQLITE_DONE)
            break;
        checkRow(resultCode);
        sqlite3_int64 vectorId = sqlite3_column_int64(stmt, 0);
        sqlite3_int64 runId = sqlite3_column_int64(stmt, 1);
        std::string attrName = (const char *)sqlite3_column_text(stmt, 2);
        std::string attrValue = (const char *)sqlite3_column_text(stmt, 3);
        auto it = sqliteVectorIdToVectorIdx.find(vectorId);
        if (it == sqliteVectorIdToVectorIdx.end())
            error("Invalid vectorId in vectorAttr table");
        VectorResult& vec = fileRunMap.at(runId)->fileRef->vectorResults.at(sqliteVectorIdToVectorIdx.at(vectorId));
        vec.setAttribute(attrName, attrValue);
    }
    finalizeStatement();
}

void SqliteResultFileLoader::cleanupDb()
{
    if (db != nullptr) {
        sqlite3_close(db);
        sqlite3_finalize(stmt);
        db = nullptr;
        stmt = nullptr;
    }
}

ResultFile *SqliteResultFileLoader::loadFile(const char *fileName, const char *fileSystemFileName, bool reload)
{
    try {
        fileRef = resultFileManager->addFile(fileName, fileSystemFileName, ResultFile::FILETYPE_SQLITE, false);
        checkOK(sqlite3_open_v2(fileSystemFileName, &db, SQLITE_OPEN_READONLY, 0));
        loadRuns();
        loadRunAttrs();
        loadRunParams();
        loadScalars();
        loadVectors();
        loadHistograms();
    }
    catch (std::exception&) {
        cleanupDb();
        try {
            if (fileRef)
                resultFileManager->unloadFile(fileRef);
        } catch (...) {}

        throw;
    }
    cleanupDb();
    return fileRef;
}

}  // namespace scave
}  // namespace omnetpp
