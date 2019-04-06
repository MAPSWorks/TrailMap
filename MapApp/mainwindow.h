/****************************************************************************
**
** Trolltech hereby grants a license to use the Qt/Eclipse Integration
** plug-in (the software contained herein), in binary form, solely for the
** purpose of creating code to be used with Trolltech's Qt software.
**
** Qt Designer is licensed under the terms of the GNU General Public
** License versions 2.0 and 3.0 ("GPL License"). Trolltech offers users the
** right to use certain no GPL licensed software under the terms of its GPL
** Exception version 1.2 (http://trolltech.com/products/qt/gplexception).
**
** THIS SOFTWARE IS PROVIDED BY TROLLTECH AND ITS CONTRIBUTORS (IF ANY) "AS
** IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
** TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
** PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
** OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
** EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
** PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
** PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
** LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
** NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
** SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** Since we now have the GPL exception I think that the "special exception
** is no longer needed. The license text proposed above (other than the
** special exception portion of it) is the BSD license and we have added
** the BSD license as a permissible license under the exception.
**
****************************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "settings.h"
#include "PoiMapWidget.h"
#include "GpxWriter.h"
#include "Logger.h"
#include "LoggerWidget.h"
#include "PoiListWidget.h"
#include "controller.h"
#include "imapadapter.h"

#include <QAction>
#include <QSettings>
#include <QtXml/QDomDocument>
#include <QStackedWidget>
#include <QProgressDialog>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    void setupWindow();
    void setupWidgets();
    void setupMenu();
    void setupMenuDesktop();
    void setupMenuAndroid();
    void buildPluginMenu(QMenu* menu);
    void buildAdapterMenu(QMenu* menu);
    void buildBookmarksMenu(QMenu* menu);
    void buildToolsMenu(QMenu* menu);
    void setupSoftKeys();
    
public slots:
    void testGps();
    void toggleFullscreen();

    void enableGps();
    void disableGps();
    
    void enableBacklight();
    void disableBacklight();
    
    void startTrace();
    void stopTrace();
    void resetTrace();
    void addPoi(bool);
    void editPoi(bool);
    void deletePoi(bool);
    void addBookmark();

    void zoomIn(bool);
    void zoomOut(bool);
    
    //void positionUpdated(const QGeoPositionInfo& update);
    void locationChanged(QPointF location);

    void gotoBookmark();
    
    void newGpx();
    void OpenGpx();
    void selectAdapter();
    void selectWorkingFolder();

    void cycleView();
    void centerToGPS();

    void progress(int value);
    void progressCompleted();
    void mapDataChanged();
    void invalidateMapCache();
    void cancelSelectAdapter();
    void SaveAsDocument();
    void saveDocument();
    void copy();
    
public:
    void showNote(const QString& message);
    
private:
    void openAdapter(const QString& name);
    void OpenGpx(const QString& path);
    void setFileName(const QString &fileName);
    
    void lockKeys();
    void refreshRSK();
    void initIcons();

    bool saveDocumentQuery();
    void updateDocumentControlsEnabled(bool enable);

protected:
    QString fileSelector(const QString& fileFilter) const;
    QString urlSelector();
    void saveSettings();
    
protected:
    void timerEvent(QTimerEvent* event);
    void keyPressEvent(QKeyEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void mouseDoubleClickEvent(QMouseEvent* event);
    void contextMenuEvent(QContextMenuEvent *event);
    void closeEvent(QCloseEvent *event);

private:
    //IMapAdapter* mAdapter;  // REF:
    
    Settings mSettings;
    Controller mController;


    //QGeoPositionInfoSource* mPositioningSource;
    
    QStackedWidget* mStackedWidget;
    PoiMapWidget* mMapWidget;
    LoggerWidget* mLoggerWidget;
    PoiListWidget* mPoiListWidget;

    QPushButton* mButtonZoomIn;
    QPushButton* mButtonZoomOut;

    QProgressDialog* mProgressDialog;
    
    QAction* mDisableGpsAction;
    QAction* mEnableGpsAction;
    QAction* mDisableBacklight;
    QAction* mEnableBacklight;
    QAction* mDisableTracing;
    QAction* mEnableTracing;
    QAction* mActionSaveAs;
    QAction* mActionSave;

    //RSK
    QAction* mNextPage;
    QAction* mCenterToGPS;

    // GPS and tracing
    bool mTracingEnabled;
    bool mFollowGPS;
    GpxWriter* mGpxWriter;
    
    Logger* mLogger;
    
    //Poi* mGpsPoi;   // BYREF: GPS indicator
    Poi* mHomePoi;  // BYREF: Start of route indicator
    QPixmap mIconGPS;
    QPixmap mIconHome;
    QPixmap mIconUserPoi;
    QPixmap mIconDefault;

    //QList<Poi> mListBookmarks;      // TODO: remove ?

    QMenu* mMenuBookmarks;
    QString mDefaultDir;
};

#endif // MAINWINDOW_H
