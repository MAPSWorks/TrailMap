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

#include "mainwindow.h"

#include "Config.h"
#include "GpxParser.h"
//#include "Coordinates.h"
#include "utility.h"

#include <QApplication>
#include <QMenu>
#include <QMenuBar>
#include <QFile>
#include <QTextStream>
#include <QFileDialog>
#include <QtXml/QXmlInputSource>
#include <QDateTime>
#include <QKeyEvent>
#include <QLabel>
#include <QMessageBox>
#include <QInputDialog>
#include <QErrorMessage>
#include <QPushbutton>
#include <QClipboard>
#include <QTimer>
#include <QDebug>

// CONSTANTS
const int KPositioningUpdateInterval = 5000; // In ms.
const QSize KButtonSize(128, 128);


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      //mPositioningSource(NULL),
      mTracingEnabled(false),
      mGpxWriter(NULL),
      mLogger(NULL),
      //mGpsPoi(NULL),
      mHomePoi(NULL),
      mFollowGPS(true),
      mMapWidget(NULL),
      mLoggerWidget(NULL),
      mPoiListWidget(NULL),
      mProgressDialog(NULL)
{
    QString adapterName = mSettings.getAdapter();
    mSettings.useAdapter(adapterName);

    mController.newDocument();

    connect(&mController, SIGNAL(locationChanged(QPointF)), this, SLOT(locationChanged(QPointF)));

    setWindowTitle(APP_NAME);

    setupWidgets();

    // Default values
    //mDisableGpsAction->setEnabled(false);
    //mDisableBacklight->setEnabled(false);
    //mDisableTracing->setEnabled(false);
    
    // Timer
    startTimer(1000);

    // Focus
    mMapWidget->setFocus();

    // Enable GPS by default.
    enableGps();

    // Icons (must be initialized before OpenGpx() calls to ensure that
    // the GPS indicator always shows at the top of Z-order).
    initIcons();

    // Settings
    OpenGpx(mSettings.getGPX());

    setupWindow();

//    openCompleted();

    qDebug() << "Setup UI DONE";
}

MainWindow::~MainWindow()
{
    mMapWidget->adapter()->unload();

    saveSettings();

    mController.saveBookmarks();

    delete mGpxWriter;
    delete mLogger;

    if(mProgressDialog != NULL)
    {
        delete mProgressDialog;
    }
}

void MainWindow::setupWindow()
{
    resize(mSettings.getWindowSize());

    if(mSettings.getWindowState() != Qt::WindowMinimized)
    {
        Qt::WindowStates s = mSettings.getWindowState();
        setWindowState(s);
    }
    else
    {
        setWindowState(Qt::WindowNoState);
    }
}

void MainWindow::setupWidgets()
{
    // Map widget
    mIconGPS.load(":/icons/ball_red.png");
    mIconDefault.load(":/icons/placemark.png");
    mMapWidget = new PoiMapWidget(mIconGPS, mIconDefault, this);

    mButtonZoomIn = new QPushButton(mMapWidget);
    mButtonZoomOut = new QPushButton(mMapWidget);

    QString adapterName = mSettings.getAdapter();
    if(adapterName.isEmpty())
    {
        // Use defaults
        const QStringList adapters = mController.adapterList();
        if(adapters.isEmpty())
        {
            qDebug() << "No adapters!";
            return;
        }
        adapterName = adapters.first();
        mSettings.saveAdapter(adapterName);
        mSettings.useAdapter(adapterName);
        mSettings.saveDataFolder("");   // Unknown
        qDebug() << "Using first adapter:" <<adapterName;
    }

    openAdapter(adapterName);

    mMapWidget->setRoutePoints(mController.routePoints());

    // Page navigator
    mStackedWidget = new QStackedWidget(this);
    setCentralWidget(mStackedWidget);

    // Map page
    mStackedWidget->addWidget(mMapWidget);

    mButtonZoomIn->setIcon(QIcon(":/icons/button.png"));
    mButtonZoomIn->setIconSize(KButtonSize);
    mButtonZoomIn->setGeometry(0, 0, KButtonSize.width(), KButtonSize.height());
    mButtonZoomIn->setFlat(true);

    mButtonZoomOut->setIcon(QIcon(":/icons/button.png"));
    mButtonZoomOut->setIconSize(KButtonSize);
    mButtonZoomOut->setGeometry(0, KButtonSize.height(), KButtonSize.width(), KButtonSize.height());
    mButtonZoomOut->setFlat(true);

    //mButtonZoomIn->hide();
    //mButtonZoomOut->hide();

    connect(mButtonZoomIn, SIGNAL(clicked(bool)), this, SLOT(zoomIn(bool)));
    connect(mButtonZoomOut, SIGNAL(clicked(bool)), this, SLOT(zoomOut(bool)));
}

void MainWindow::setupMenu()
{
    menuBar()->clear();

#if defined(Q_OS_SYMBIAN) || defined(Q_WS_SIMULATOR)
    setupSoftKeys();
#elif defined(Q_OS_ANDROID)
    setupMenuAndroid();
#else
    setupMenuDesktop();
#endif
}

void MainWindow::setupMenuDesktop()
{
    QMenu* menuFile = menuBar()->addMenu(tr("File"));
    QMenu* menuEdit = menuBar()->addMenu(tr("Edit"));
    QMenu* menuView = menuBar()->addMenu(tr("View"));

    // Map plugin menu
    QMenu* menuMap = menuBar()->addMenu(tr("Map"));
    buildPluginMenu(menuMap);

    // TODO: QMenu* menuBookmarks = menuBar()->addMenu(tr("Bookmarks"));
    QMenu* menuHelp = menuBar()->addMenu(tr("Help"));

    // Construct File menu
    QMenu* menuNewMap = menuFile->addMenu(tr("Open"));
    buildAdapterMenu(menuNewMap);

    menuFile->addSeparator();
    QAction* actionNewGpx = menuFile->addAction(tr("New"));
    QAction* actionOpenGpx = menuFile->addAction(tr("Open..."));
    menuFile->addSeparator();
    mActionSaveAs = menuFile->addAction(tr("Save As..."));
    mActionSave = menuFile->addAction(tr("Save"));
    QAction* actionPrint = menuFile->addAction(tr("Print..."));
    menuFile->addSeparator();
    QAction* actionExit = menuFile->addAction(tr("Exit"));

    // Construct Edit menu
    QAction* actionCopy = menuEdit->addAction("Copy\tCtrl+C");

    // Construct View menu
    QAction* actionFullscreen = menuView->addAction("Full screen\tF11");

    // Construct Bookmarks menu
    // TODO:
    //buildBookmarksMenu(menuBookmarks);

    // Construct Help menu
    menuHelp->addAction("Help")->setEnabled(false);     // TODO: disabled for now
    menuHelp->addSeparator();
    menuHelp->addAction("About")->setEnabled(false);    // TODO: disabled for now

    // Set shortcuts
    actionNewGpx->setShortcut(Qt::CTRL | Qt::Key_N);
    actionOpenGpx->setShortcut(Qt::CTRL | Qt::Key_O);
    mActionSave->setShortcut(Qt::CTRL | Qt::Key_S);
    actionPrint->setShortcut(Qt::CTRL | Qt::Key_P);
    actionExit->setShortcut(Qt::Key_Escape);
    actionFullscreen->setShortcut(Qt::Key_F11);
    actionCopy->setShortcut(Qt::CTRL | Qt::Key_C);

    // Connect signals
    connect(actionExit, SIGNAL(triggered()), SLOT(close()));
    connect(actionFullscreen, SIGNAL(triggered()), SLOT(toggleFullscreen()));
    connect(actionNewGpx, SIGNAL(triggered()), SLOT(newGpx()));
    connect(actionOpenGpx, SIGNAL(triggered()), SLOT(OpenGpx()));
    connect(mActionSaveAs, SIGNAL(triggered()), SLOT(SaveAsDocument()));
    connect(mActionSave, SIGNAL(triggered()), SLOT(saveDocument()));
    connect(actionCopy, SIGNAL(triggered()), SLOT(copy()));

    // TODO: these menu choices are still unimplemented and therefore disabled.
    //actionOpenGpx->setEnabled(false);
    mActionSaveAs->setEnabled(false);
    mActionSave->setEnabled(false);
    actionPrint->setEnabled(false);

#ifdef QT_DEBUG
    QMenu* menuTest = menuBar()->addMenu("TEST!");
    QAction* actionTestGps = menuTest->addAction("TEST GPS");
    connect(actionTestGps, SIGNAL(triggered()), SLOT(testGps()));
#endif
}

void MainWindow::setupMenuAndroid()
{
    qDebug() << "Setup menus for Android.";

    // TODO: Qt selection dialog
    // does not work very well.
    QMenu* menuOpen = menuBar()->addMenu(tr("Open map"));
    buildAdapterMenu(menuOpen);

    // Map plugin menu
    QMenu* menuMap = menuBar()->addMenu(tr("Map"));

    // Open GPX menu choice
    QAction* actionOpenGpx = menuMap->addAction(tr("Open GPX..."));
    connect(actionOpenGpx, SIGNAL(triggered()), SLOT(OpenGpx()));

    // Add placemark here menu choice.
    QAction* actionAddPoi = menuMap->addAction(tr("Add POI..."));
    connect(actionAddPoi, SIGNAL(triggered(bool)), this, SLOT(addPoi(bool)));

    buildPluginMenu(menuMap);

    mMenuBookmarks = menuBar()->addMenu(tr("Bookmarks"));
    buildBookmarksMenu(mMenuBookmarks);

    qDebug() << "Menus DONE";
}

void MainWindow::buildPluginMenu(QMenu* menu)
{
    QAction* actionSelectFolder = menu->addAction(tr("Select working folder..."));
    connect(actionSelectFolder, SIGNAL(triggered()), this, SLOT(selectWorkingFolder()));

    QAction* separator = menu->addSeparator();
    const int menuItemCount = menu->actions().count();

    mController.setupAdapterMenu(*menu);

    if(menu->actions().count() == menuItemCount)
    {
        // Plugin did not add menu items, no need to have separator.
        menu->removeAction(separator);
        delete separator;
    }

#if !defined(Q_OS_ANDROID)
    menu->addSeparator();
    mMenuBookmarks = menu->addMenu(tr("Bookmarks"));
    buildBookmarksMenu(mMenuBookmarks);
#endif
}

void MainWindow::buildAdapterMenu(QMenu* menu)
{
    const QStringList adapters = mController.adapterList();

    foreach(QString adapter, adapters)
    {
        QAction* actionAdapter = menu->addAction(adapter + "...");
        actionAdapter->setData(QVariant(adapter));
        connect(actionAdapter, SIGNAL(triggered()), SLOT(selectAdapter()));
    }
}

void MainWindow::setupSoftKeys()
{
    QMenu* menu = new QMenu(this);

    // Build bookmarks submenu

    // TODO:
    //QMenu* menuBookmarks = menu->addMenu(tr("Bookmarks"));
    //buildBookmarksMenu(menuBookmarks);

    // Menu options
    /*
    mEnableGpsAction = menu->addAction(tr("Enable GPS"), this, SLOT(enableGps()));
    mDisableGpsAction = menu->addAction(tr("Disable GPS"), this, SLOT(disableGps()));
    mEnableBacklight = menu->addAction(tr("Backlight ON"), this, SLOT(enableBacklight()));
    mDisableBacklight = menu->addAction(tr("Backlight OFF"), this, SLOT(disableBacklight()));
    */
    // TODO: menu->addAction(tr("Save location..."), this, SLOT(addPoi()));
    menu->addAction(tr("Avaa GPX..."), this, SLOT(OpenGpx()));

    //mEnableTracing = menu->addAction(tr("Start trace"), this, SLOT(startTrace()));
    //mDisableTracing = menu->addAction(tr("Stop trace"), this, SLOT(stopTrace()));
    mDisableTracing = menu->addAction(tr("Reset route"), this, SLOT(resetTrace()));
    //menu->addAction(tr("Send location as SMS..."), this, SLOT(SendLocation()));

    QMenu* menuOpenMap = menu->addMenu(tr("Open Map"));
    buildAdapterMenu(menuOpenMap);
    menu->addAction(tr("Exit"), this, SLOT(close()));
/*
    // LSK
    QAction* options = new QAction(tr("Menu"), this);
    options->setSoftKeyRole(QAction::PositiveSoftKey);
    options->setMenu(menu);
    addAction(options);

    // RSK
    mNextPage = new QAction(tr("Next page"), this);
    mNextPage->setSoftKeyRole(QAction::NegativeSoftKey);
    connect(mNextPage, SIGNAL(triggered()), this, SLOT(cycleView()));

    mCenterToGPS = new QAction(tr("Location"), this);
    mCenterToGPS->setSoftKeyRole(QAction::NegativeSoftKey);
    connect(mCenterToGPS, SIGNAL(triggered()), this, SLOT(centerToGPS()));

    refreshRSK();
*/
    /*
    QAction* rsk= new QAction(tr("Next page"), this);
    rsk->setSoftKeyRole(QAction::NegativeSoftKey);
    connect(rsk, SIGNAL(triggered()), this, SLOT(cycleView()));
    addAction(rsk);
    */
}

void MainWindow::refreshRSK()
{
    if(mFollowGPS && mStackedWidget->count() > 1)
    {
        // RSK = 'Next page'
        removeAction(mCenterToGPS);
        addAction(mNextPage);
    }
    else
    {
        // RSK = 'Center to GPS'
        removeAction(mNextPage);
        addAction(mCenterToGPS);
    }
}

void MainWindow::testGps()
{
    QPointF location(28.1392325, 68.2183648);
    locationChanged(location);
}

void MainWindow::toggleFullscreen()
{
    if(windowState() == Qt::WindowFullScreen)
        showMaximized();
    else
        showFullScreen();
}

void MainWindow::initIcons()
{
    //mIconGPS.load(":/icons/ball_red.png");
    //mGpsPoi = mMapWidget->addPoi(QPoint(), mIconGPS);

    // TODO:
    mIconHome.load(":/icons/warning_triangle.png");
    mHomePoi = mMapWidget->addPoi(mSettings.getHomeLocation(), mIconHome);

    mIconUserPoi.load(":/icons/flag.png");
}

void MainWindow::enableGps()
{
}

void MainWindow::disableGps()
{
}

void MainWindow::enableBacklight()
{
    mEnableBacklight->setEnabled(false);
    mDisableBacklight->setEnabled(true);
    
    showNote(tr("Backlight ON"));
}

void MainWindow::disableBacklight()
{
    mEnableBacklight->setEnabled(true);
    mDisableBacklight->setEnabled(false);
    
    showNote(tr("Backlight OFF"));
}

void MainWindow::startTrace()
{
}

void MainWindow::stopTrace()
{
}

void MainWindow::resetTrace()
{
}

void MainWindow::addPoi(bool)
{
    QAction* action = dynamic_cast<QAction*>(sender());
    if(action)
    {
        const QVariant data = action->data();
        QPointF location = data.toPointF();
        if(location.isNull())
        {
            location = this->mController.location();
        }

        qDebug() << "addPoi" << location;
        if(!location.isNull())
        {
            // TODO: add input for description !
            const QString title = QInputDialog::getText(this, tr("Add placemark"),
                                                 tr("Title:"), QLineEdit::Normal,
                                                 "");
            if(!title.isEmpty())
            {
                Poi placemark(location, title);
                Document* document = mController.document();
                document->addPlacemark(placemark);
                updateDocumentControlsEnabled(true);
            }
        }
    }
}

void MainWindow::editPoi(bool)
{
    QAction* action = dynamic_cast<QAction*>(sender());
    if(action)
    {
        const QVariant data = action->data();
        const QPointF location = data.toPointF();
        qDebug() << "editPoi" << location;
        if(!location.isNull())
        {
            const Poi* poi = mMapWidget->poiAt(mMapWidget->adapter()->LatLonToDisplayCoordinate(location));
            const QString title = QInputDialog::getText(this, tr("Edit placemark"),
                                                 tr("Title:"), QLineEdit::Normal,
                                                 poi->title());
            if(!title.isEmpty())
            {
                Poi* p = const_cast<Poi*>(poi);
                p->setTitle(title);
                updateDocumentControlsEnabled(true);
            }
        }
    }
}

void MainWindow::deletePoi(bool)
{
    QAction* action = dynamic_cast<QAction*>(sender());
    if(action)
    {
        const QVariant data = action->data();
        const QPointF location = data.toPointF();
        qDebug() << "deletePoi" << location;
        if(!location.isNull())
        {
            const Poi* poi = mMapWidget->poiAt(mMapWidget->adapter()->LatLonToDisplayCoordinate(location));

            QMessageBox msgBox;
            msgBox.setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
            msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
            msgBox.setIcon(QMessageBox::Warning);
            msgBox.setText(tr("Delete placemark ?"));       // TODO: maybe show POI title in dialog ?
            if(msgBox.exec() == QMessageBox::Yes)
            {
                Document* document = mController.document();
                document->removePlacemark(*poi);
            }
        }
    }
}

void MainWindow::addBookmark()
{
    QPoint location = mMapWidget->mapCenter();
    QPointF latLon = mController.currentAdapter()->DisplayCoordinateToLatLon(location);

    if(!latLon.isNull())
    {
        bool ok;
        QString text = QInputDialog::getText(this, tr("Add bookmark"),
                                             tr("Title:"), QLineEdit::Normal,
                                             "", &ok);

        if(ok)
        {
            Poi bookmark(latLon, text);

            QList<Poi>& bookmarks = mController.bookmarks();
            bookmarks.append(bookmark);
            mController.saveBookmarks();

            buildBookmarksMenu(mMenuBookmarks);
        }
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText(tr("Cannot get coordinates."));
        msgBox.exec();
    }
}

void MainWindow::zoomIn(bool)
{
    mMapWidget->zoomIn();
}

void MainWindow::zoomOut(bool)
{
    mMapWidget->zoomOut();
}

void MainWindow::gotoBookmark()
{
    QObject* s = sender();
    QAction* action = dynamic_cast<QAction*>(s);
    if(action)
    {
        const QPointF location = action->data().toPointF();
        mMapWidget->setLocation(location);
        mMapWidget->render();
    }
}

void MainWindow::newGpx()
{
    setFileName("");
    mController.newDocument();
    mSettings.saveGPX("");

    mMapWidget->setPoiSource(mController.poiSource());
    mMapWidget->setRouteSource(mController.routeSource());

    updateDocumentControlsEnabled(false);
}

void MainWindow::OpenGpx()
{
    const QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open POI File"), mDefaultDir, tr("GPX Files (*.gpx);;LMX Files (*.lmx)"));

    if(!fileName.isEmpty())
    {
        if(mController.document()->hasChanged())
        {
           saveDocumentQuery();
        }

        mMapWidget->clearPois();

        OpenGpx(fileName);

        // Open this file the next time the app is started.
        mSettings.saveGPX(fileName);

        mDefaultDir = QFileInfo(fileName).path();
    }
}

void MainWindow::selectAdapter()
{
    QObject* s = sender();
    QAction* action = dynamic_cast<QAction*>(s);
    if(action)
    {
        saveSettings();

        QString adapterName = action->data().toString();
        openAdapter(adapterName);
        mMapWidget->render();
        mMapWidget->update();
    }
}

void MainWindow::openAdapter(const QString& adapterName)
{
    if(mMapWidget->adapter() != NULL)
    {
        mController.saveBookmarks();

        mMapWidget->adapter()->unload();        // TODO: no need to call?
        mMapWidget->setAdapter(NULL);
    }

    IMapAdapter* mapAdapter = mController.adapterByName(adapterName);
    Q_ASSERT(mapAdapter != NULL);
    connect(mapAdapter, SIGNAL(progress(int)), this, SLOT(progress(int)));
    connect(mapAdapter, SIGNAL(openCompleted()), this, SLOT(progressCompleted()));
    connect(mapAdapter, SIGNAL(dataChanged()), this, SLOT(mapDataChanged()));
    connect(mapAdapter, SIGNAL(invalidate()), this, SLOT(invalidateMapCache()));

    mSettings.useAdapter(adapterName);
    QString dataFolder = mSettings.getDataFolder();
    if(dataFolder.isEmpty())
    {
        // Use default
        dataFolder = Utility::findWorkingFolder();

        // Create folder for plugin.
        // TODO: Should create folder only when needed i.e. when plugin needs to save data.
        dataFolder = dataFolder + adapterName;
        QDir makedir;
        makedir.mkpath(dataFolder);
    }

    mapAdapter->openDataSource(dataFolder);
    mController.loadBookmarks(dataFolder);

    const QString& copyrightNotice = mapAdapter->copyrightNotice();
    if(!copyrightNotice.isEmpty())
    {
        QMessageBox::information(this, APP_NAME, copyrightNotice);
    }

    //mSettings.saveGPX("");
    mMapWidget->clearPois();

    mMapWidget->setAdapter(mapAdapter);

    // Get location from settings.
    QPoint location = mSettings.getLocation();
    if(!location.isNull())
    {
        qDebug() << "Set location from settings:" << location;
        mMapWidget->setLocation(location);
        mapAdapter->setZoom(mSettings.getZoom());
    }

    qDebug() << "Adapter set:" << adapterName;
    qDebug() << "Data folder:" << dataFolder;
    mSettings.saveAdapter(adapterName);


    // Re-build menu.
    setupMenu();

#if defined(Q_OS_ANDROID)
    /*
    if(mapAdapter->maxZoom() == mapAdapter->minZoom())
    {
        mButtonZoomIn->hide();
        mButtonZoomOut->hide();
    }
    else
    {
        mButtonZoomIn->show();
        mButtonZoomOut->show();
    }
    */
#else
    mButtonZoomIn->hide();
    mButtonZoomOut->hide();
#endif
}

void MainWindow::selectWorkingFolder()
{
    QString workingFolder = QFileDialog::getExistingDirectory(NULL, tr("Select folder:"));

    if(!workingFolder.isEmpty())
    {
        qDebug() << "User set data folder:" << workingFolder;

        mController.saveBookmarks();

        IMapAdapter* adapter = mController.currentAdapter();
        adapter->openDataSource(workingFolder);
        mMapWidget->render();
        mMapWidget->update();
        mController.loadBookmarks(workingFolder);

        // Re-build menu.
        setupMenu();
    }
}

QString MainWindow::fileSelector(const QString& fileFilter) const
{
    return QFileDialog::getOpenFileName(NULL,
        tr("Open file"), "", fileFilter, NULL, QFileDialog::ReadOnly);
}

QString MainWindow::urlSelector()
{
    bool ok;
    QString url = QInputDialog::getText(this, tr("Enter data source URL"),
                                        tr("URL:"), QLineEdit::Normal,
                                        "http://", &ok, Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint);
    if(ok && !url.isEmpty())
    {
        return url;
    }

    return QString();
}

void MainWindow::saveSettings()
{
    mSettings.saveWindowSize(size());
    mSettings.saveWindowState(windowState());
    mSettings.saveLocation(mMapWidget->mapCenter());
    mSettings.saveZoom(mMapWidget->adapter()->zoom());
    mSettings.saveHomeLocation(mHomePoi->location());
    mSettings.saveDataFolder(mMapWidget->adapter()->dataFolder());
}

/*
void MainWindow::OpenGpx(const QString& fileName)
{
    initIcons();

    const QList<Poi> poiList = mController.OpenFile(fileName);

    mMapWidget->addLatLonPoiList(poiList, NULL);
    update();
}
*/
void MainWindow::OpenGpx(const QString& fileName)
{
    qDebug() << "Opening document:" << fileName;

    setFileName(fileName);

    initIcons();

    mController.OpenDocument(fileName);

    mMapWidget->setPoiSource(mController.poiSource());
    mMapWidget->setRouteSource(mController.routeSource());
    update();

    updateDocumentControlsEnabled(false);
}

void MainWindow::setFileName(const QString& fileName)
{
    if(fileName.isEmpty())
    {
        setWindowTitle(APP_NAME);
    }
    else
    {
        const QString windowTitle = QString("%1 - [%2]").arg(APP_NAME).arg(fileName);
        setWindowTitle(windowTitle);
    }
}

void MainWindow::showNote(const QString& message)
{
    /*
    // Symbian-specific
    TPtrC16 textPtr(reinterpret_cast<const TUint16*>(message.utf16()));
    CAknConfirmationNote* mConfNote;
    mConfNote = new (ELeave) CAknConfirmationNote( ETrue );  // waiting
    mConfNote->ExecuteLD( textPtr );  // display the note and delete it 
    */
}

void MainWindow::cycleView()
{
    int page = mStackedWidget->currentIndex();
    if(++page >= mStackedWidget->count())
        page = 0;
    mStackedWidget->setCurrentIndex(page);
}

void MainWindow::centerToGPS()
{
    const QPointF location = mController.location();
    if(!location.isNull())
    {
        mFollowGPS = true;
        mMapWidget->positionUpdate(location);
        mMapWidget->setLocation(location);
        mMapWidget->update();
        refreshRSK();
    }
    else
    {
        QMessageBox::information(this, "GPS", tr("Sijainti ei ole vielä tiedossa."));
    }
}

void MainWindow::progress(int value)
{
    if(mProgressDialog == NULL)
    {
        mProgressDialog = new QProgressDialog(tr("Please wait..."), "Cancel", 0, 100, this);
        mProgressDialog->setModal(true);
        mProgressDialog->setCancelButton(NULL);
        mProgressDialog->setWindowFlags( ( (mProgressDialog->windowFlags() | Qt::CustomizeWindowHint)& ~Qt::WindowCloseButtonHint) );
        mProgressDialog->setWindowFlags(mProgressDialog->windowFlags() & ~Qt::WindowContextHelpButtonHint);
    }

    if(!mProgressDialog->isVisible())
    {
        mProgressDialog->show();
    }
    mProgressDialog->setValue(value);
}

// Dismiss progress dialog and show map.
void MainWindow::progressCompleted()
{
    qDebug() << "Loading adapter complete.";
/*
    disconnect(this, SLOT(progressCompleted()));
    disconnect(this, SLOT(progress(int)));
*/
    mProgressDialog->hide();
    /*
    // NOTE: defaultLocation() returns tile coordinate, not pixel.
    const QPoint location = mMapWidget->adapter()->defaultLocation();
    const QPoint locPx(location.x() * mMapWidget->adapter()->tileSize().width(),
                       location.y() * mMapWidget->adapter()->tileSize().height());
    mMapWidget->setLocation(locPx);
    */
    mMapWidget->render();
    mMapWidget->update();
}

void MainWindow::mapDataChanged()
{
    mMapWidget->render();
    mMapWidget->update();
}

void MainWindow::invalidateMapCache()
{
    mMapWidget->clearCache();
}

void MainWindow::cancelSelectAdapter()
{
    qDebug() << "cancelSelectAdapter()";

    // TODO:
}

void MainWindow::locationChanged(QPointF location)
{
    mMapWidget->positionUpdate(location);
    if(mFollowGPS)
    {
        mMapWidget->setLocation(location);
    }
}

void MainWindow::buildBookmarksMenu(QMenu* menu)
{
    menu->clear();
    QAction* actionAddBookmark = menu->addAction(tr("Add..."), this, SLOT(addBookmark()));
    actionAddBookmark->setShortcut(Qt::CTRL | Qt::Key_D);

    if(mController.bookmarks().count() > 0)
    {
        menu->addSeparator();
    }

    for(int c = 0; c < mController.bookmarks().count(); c++)
    {
        const QString title(mController.bookmarks()[c].title());
        QAction* actionBookmark = new QAction(title, this);
        QPointF latLon = mController.bookmarks()[c].latLon();
        actionBookmark->setData(QVariant(latLon));
        connect(actionBookmark, SIGNAL(triggered()), SLOT(gotoBookmark()));
        menu->addAction(actionBookmark);
    }

    //actionAddBookmark->setEnabled(false);       // TODO: not implemented
}

void MainWindow::lockKeys()
{
}

void MainWindow::timerEvent(QTimerEvent* /*event*/)
{
    if(mLogger && mLoggerWidget && mLoggerWidget->isVisible())
    {
        mLoggerWidget->setDuration(mLogger->duration());
        mLoggerWidget->setDistance(mLogger->distance());
        mLoggerWidget->setAvgSpeed(mLogger->averageVelocity());
    }
}

void MainWindow::keyPressEvent(QKeyEvent* event)
{
    switch(event->key())
    {
        case 16777362:  // TODO: use enum!
            mMapWidget->cycleZoom();
            break;
        default:
            QMainWindow::keyPressEvent(event);
            break;
    }
}

void MainWindow::mousePressEvent(QMouseEvent* event)
{
    mFollowGPS = false;
    event->accept();

#if defined(Q_OS_SYMBIAN)
    refreshRSK();
#endif
}

void MainWindow::mouseReleaseEvent(QMouseEvent* event)
{
}

void MainWindow::mouseDoubleClickEvent(QMouseEvent* event)
{
#if defined(Q_OS_SYMBIAN) || defined(Q_WS_SIMULATOR) || defined(Q_OS_ANDROID)
    mMapWidget->cycleZoom();
#endif
}

void MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
#if defined(Q_OS_WIN)
    const QPoint linearLocation = mMapWidget->location() + mMapWidget->mapFromGlobal(event->globalPos());
    const Poi* poi = mMapWidget->poiAt(linearLocation);

    QMenu menu(this);

    QAction* actionNew = new QAction(tr("Add placemark..."), &menu);
    QAction* actionEdit = new QAction(tr("Edit..."), &menu);
    QAction* actionDelete = new QAction(tr("Delete..."), &menu);

    connect(actionNew, SIGNAL(triggered(bool)), this, SLOT(addPoi(bool)));
    connect(actionEdit, SIGNAL(triggered(bool)), this, SLOT(editPoi(bool)));
    connect(actionDelete, SIGNAL(triggered(bool)), this, SLOT(deletePoi(bool)));

    actionEdit->setEnabled(poi != NULL);
    actionDelete->setEnabled(poi != NULL);

    const QPointF location = mMapWidget->adapter()->DisplayCoordinateToLatLon(linearLocation);
    actionNew->setData(QVariant(location));
    actionEdit->setData(QVariant(location));
    actionDelete->setData(QVariant(location));

    menu.addAction(actionNew);
    menu.addAction(actionEdit);
    menu.addAction(actionDelete);

    menu.exec(event->globalPos());
#endif
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if(this->mController.document()->hasChanged())
    {
        saveDocumentQuery();
    }
}

bool MainWindow::saveDocumentQuery()
{
    QMessageBox::StandardButton dialog;
    dialog = QMessageBox::warning(this, "Confirmation", "Save changes to file?", QMessageBox::Yes | QMessageBox::No);
    if(dialog == QMessageBox::Yes)
    {
        saveDocument();
    }

    return dialog == QMessageBox::Yes;
}

void MainWindow::SaveAsDocument()
{
    Document* document = mController.document();
    const QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"), document->filename(), tr("GPX Files (*.gpx)"));

    if(!fileName.isEmpty())
    {
        mController.save(*document, fileName);
        updateDocumentControlsEnabled(false);
    }
}

void MainWindow::saveDocument()
{
    Document* document = mController.document();
    const QString fileName = document->filename();

    if(fileName.isEmpty())
    {
        SaveAsDocument();
    }
    else
    {
        mController.save(*document, fileName);
        updateDocumentControlsEnabled(false);
    }
}

void MainWindow::copy()
{
    QClipboard* clipboard = QApplication::clipboard();
    clipboard->setPixmap(mMapWidget->grab());
}

void MainWindow::updateDocumentControlsEnabled(bool enable)
{
#if !defined(Q_OS_ANDROID)
    mActionSaveAs->setEnabled(enable);
    mActionSave->setEnabled(enable);
#endif
}
