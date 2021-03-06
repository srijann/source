//----------------------------------------------------------------------------
// ObjectComponents
// Copyright (c) 1994, 1996 by Borland International, All Rights Reserved
//
//   Definition of Compound Document TOcView Class
//----------------------------------------------------------------------------
#if !defined(OCF_OCVIEW_H)
#define OCF_OCVIEW_H

#if !defined(OCF_OCDOC_H)
# include <ocf/ocdoc.h>      // Client related
#endif

#if !defined(OCF_OCAPP_H)
# include <ocf/ocapp.h>      // Client related
#endif

//
// Classes referenced
//
class _ICLASS TOcControl;
class _ICLASS TOcStorage;
class _ICLASS TOcDataProvider;
class         TRegList;

//
// OCF Mixin classes for users derived application class
//
#if !defined(_OCMCLASS)
# define _OCMCLASS
#endif
class _OCMCLASS TOcContainerHost;
class _OCMCLASS TOcServerHost;

//
// View options flags, set with SetOption
//
enum TOcViewOptions {
  voNoInPlace        = 0x0001, // If FALSE, allow inplace activate in container
  voNoNestedInPlace  = 0x0002, // If FALSE, allow nested inplace activate " "
  voNoInPlaceServer  = 0x0004, // If FALSE, allow server to activate inplace
};

//
// class TOcFormat
// ~~~~~ ~~~~~~~~~
// Clipboard format wrapper
//
class TOcFormat {
  public:
    TOcFormat();
    TOcFormat(uint fmtId, char far* fmtName, char far* fmtResultName,
              uint fmtMedium, bool fmtIsLinkable,
              uint aspect = 1, uint direction = 1);

    void operator =(const TOcFormatInfo&);
    bool operator ==(const TOcFormat& other) {return ToBool(&other == this);}
    void SetFormatId(uint id) {Id = id;}
    void SetFormatName(char far* name, TOcApp& ocApp);
    void SetFormatName(uint id, TOcApp& ocApp);
    void SetMedium(uint medium) {Medium = (ocrMedium)medium;}
    void SetAspect(uint aspect) {Aspect = aspect;}
    void SetDirection(uint direction) {Direction = direction;}
    void SetLinkable(bool link = true) {IsLinkable = link;}
    void Disable(bool disable = true) {Disabled = disable;}

    uint GetFormatId() const {return Id;}
    char far* GetRegName() {return RegName;}
    char far* GetFormatName() {return Name;}
    uint GetMedium() const {return Medium;}
    uint GetAspect() const {return Aspect;}
    uint GetDirection() const {return Direction;}
    bool IsDisabled() const {return Disabled;}

    void GetFormatInfo(TOcFormatInfo far& f);

  private:
    uint      Id;
    char      RegName[32];
    char      Name[32];
    char      ResultName[32];
    ocrMedium Medium;
    bool      IsLinkable;

    uint Aspect;      // last three data members added & maintained by ocf
    uint Direction;
    bool Disabled;    // Format not available
};

//
// class TOcFormatList
// ~~~~~ ~~~~~~~~~~~~~
// Container of clipboard formats with iterator
//
class TOcFormatList : private TICVectorImp<TOcFormat> {
  public:
    TOcFormatList();
   ~TOcFormatList();
    void operator delete(void* ptr) {TStandardAllocator::operator delete(ptr);}

    TOcFormat*&  operator [](unsigned index) {return Base::operator[](index);}
    void         Clear(int del = 1) {Base::Flush(del);}
    int          Add(TOcFormat* format) {return Base::Add(format);}
    int          IsEmpty() const {return Base::IsEmpty();}
    unsigned     Find(const TOcFormat* format) const {return Base::Find(format);}
    virtual uint Count() const {return Base::Count();}
    int          Detach(const TOcFormat* format, int del = 0)
                    {return Base::Detach(Find(format), del);}
    TOcFormat* Find(uint const id) const;

  private:
    typedef TICVectorImp<TOcFormat> Base;
    friend class TOcFormatListIter;
};

//
// class TOcFormatListIter
// ~~~~~ ~~~~~~~~~~~~~~~~~
class TOcFormatListIter : public TICVectorIteratorImp<TOcFormat> {
  private:
    typedef TICVectorIteratorImp<TOcFormat> Base;
  public:
    TOcFormatListIter(const TOcFormatList& c) : Base(c) {}
    operator   int() const {return Base::operator int();}
    TOcFormat* Current() const {return Base::Current();}
    TOcFormat* operator ++(int) {return Base::operator ++(0);}
    TOcFormat* operator ++() {return Base::operator ++();}
    void       Restart() {Base::Restart();}
    void       Restart(unsigned start, unsigned stop) {Base::Restart(start, stop);}
};

//
// class TOcView
// ~~~~~ ~~~~~~~
// The TOcView partner is a container (viewer) of a given (server/client)
// document.
//
class _ICLASS TOcView : public TUnknown,
                        public IBContainer,
                        public IBContains,
                        public IBDropDest {
  public:
    TOcView(TOcDocument& doc, TOcContainerHost* ch, TOcServerHost* sh,
            TRegList* regList=0, IUnknown* outer=0);

    TOcView(TOcDocument& doc, TRegList* regList=0, IUnknown* outer=0);

    void         SetupWindow(HWND hWin, bool embedded = false);

    virtual void ReleaseObject();

    TOcDocument& GetOcDocument() {return OcDocument;}

    // Helper functions
    //
    IBRootLinkable* GetLinkable() {return BLDocumentI;}

    // Clipboard support
    //
    bool         RegisterClipFormats(TRegList& regList);
    bool         BrowseClipboard(TOcInitInfo& initInfo);
    bool         PasteNative(TOcInitInfo& init, TPoint far* where = 0);
    bool         Paste(bool linking = false);
    void         SetOcData(TOcDataProvider* ocData = 0) {OcData = ocData;}
    TOcDataProvider* GetOcData() {return OcData;}

    // View related
    //
    TPoint       GetOrigin() const {return Origin;}
    TRect        GetWindowRect() const;
    void         ScrollWindow(int dx, int dy);
    void         InvalidatePart(TOcPartChangeInfo& changeInfo);
    virtual void Rename();
    TOcLinkView* GetDocLink(const char far* name=0);
    bool         IsOptionSet(uint32 option) const;
    void         SetOption(uint32 bit, bool state);

    // Get/Set active part
    //
    TOcPart*     GetActivePart() {return ActivePart;}
    void         SetActivePart(TOcPart* part) {ActivePart = part;} // Internal use
    bool         ActivatePart(TOcPart* part);

    // IBDocument pass-thrus
    //
    void         EvResize();
    void         EvActivate(bool activate);
    virtual void EvClose();
    virtual bool EvSetFocus(bool set);
    bool         EnumLinks(IBLinkInfo far* far*);
    bool         BrowseLinks();
    bool         UpdateLinks();

    // Object reference & lifetime managment
    // For internal OCF use only
    //
    ulong   _IFUNC AddRef() {return GetOuter()->AddRef();}
    ulong   _IFUNC Release() {return GetOuter()->Release();}
    HRESULT _IFUNC QueryInterface(const GUID far& iid, void far*far* iface)
                     {return GetOuter()->QueryInterface(iid, iface);}
  protected:
    // IBContains implementation for BOle to use
    //
    HRESULT _IFUNC Init(LPCOLESTR);
    HRESULT _IFUNC GetPart(IBPart far* far*, LPCOLESTR);

    // IBDataNegotiator implementation
    //
    uint     _IFUNC CountFormats();
    HRESULT  _IFUNC GetFormat(uint, TOcFormatInfo far*);

    // IBWindow implementation
    //
    HWND     _IFUNC GetWindow();
    HRESULT  _IFUNC GetWindowRect(TRect far* r);
    LPCOLESTR _IFUNC GetWindowTitle();
    void     _IFUNC AppendWindowTitle(LPCOLESTR title);
    HRESULT  _IFUNC SetStatusText(LPCOLESTR text);
    HRESULT  _IFUNC RequestBorderSpace(const TRect far*);
    HRESULT  _IFUNC SetBorderSpace(const TRect far*);
    HRESULT  _IFUNC InsertContainerMenus(HMENU, TOcMenuWidths far*);
    HRESULT  _IFUNC SetFrameMenu(HMENU);
    void     _IFUNC RestoreUI();
    HRESULT  _IFUNC Accelerator(MSG far*);
    HRESULT  _IFUNC GetAccelerators(HACCEL far*, int far*);

    // IBDropDest implementation
    //
    HRESULT  _IFUNC Drop(TOcInitInfo far*, TPoint far*, const TRect far*);
    void     _IFUNC DragFeedback(TPoint far*, const TRect far*, TOcMouseAction, uint, HRESULT& hr);
    HRESULT  _IFUNC Scroll(TOcScrollDir scrollDir);
    HRESULT  _IFUNC GetScrollRect(TRect far*);

    // IBContainer implementation
    //
    HRESULT  _IFUNC FindDropDest(TPoint far*, IBDropDest far* far*);
    HRESULT  _IFUNC AllowInPlace();
    HRESULT  _IFUNC BringToFront();

  protected:
    uint32 ForwardEvent(int eventId, const void far* param);
    uint32 ForwardEvent(int eventId, uint32 param = 0);

  protected:
   ~TOcView();
    void Shutdown();// called from derived class destructor to release helpers

    // TUnknown overrides
    //
    HRESULT      QueryObject(const IID far& iid, void far* far* iface);

    // BOle side support
    //
    IUnknown*       BDocument;   // Document helper object
    IBDocument*     BDocumentI;  // Document interface on the document
    IUnknown*       BContainer;  // Container helper object
    IBRootLinkable* BLDocumentI; // RootLinkable interface on the container

    // OC wiring
    //
    TOcApp&         OcApp;        // Our OC application object
    TOcDocument&    OcDocument;   // Our OC document object

    TOcContainerHost* ContainerHost; // The hosting app's container object
    TOcServerHost*    ServerHost;    // The hosting app's server object

    // App side support
    //
    TOcPart*        ActivePart;   // Currently active part, if any

    uint32          Options;

#if defined(BI_DATA_NEAR)
    TPoint&          Origin;       // Origin of view topleft relative to the document
    TSize&           Extent;       // Extent of view's perception of document?
    string&          WinTitle;     // Merged window title string
    TOcFormatList&   FormatList;   // list of clipboard formats supported
    TString&         OrgTitle;     // Original window title string
#else
    TPoint           Origin;
    TSize            Extent;
    string           WinTitle;
    TOcFormatList    FormatList;
    TString          OrgTitle;     // Original window title string
#endif
    int              LinkFormat;   // adjustment for format count
    TRegList*        RegList;
    TOcDataProvider* OcData;       // Dataprovider representing this view

  friend class _ICLASS TOcControl;
  friend class _ICLASS TOcControlEvent;
  friend class _ICLASS TOcPart;
  friend class _ICLASS TOcDataProvider;
  friend class _ICLASS TOcLinkView;
};

//----------------------------------------------------------------------------

//
// For viewdrop & viewdrag
//
struct TOcDragDrop {
  TOcInitInfo far*  InitInfo;  // ViewDrop event only, else 0
  TPoint*           Where;
  TRect*            Pos;
};

//
// For part adornment painting over part, & painting views
//
struct TOcViewPaint {
  HDC        DC;
  TRect*     Pos;
  TRect*     Clip;
  TOcAspect  Aspect;
  bool       PaintSelection;  // paint the selection only
  TString*   Moniker;         // moniker if any
  void*      UserData;        // User data

};

//
// Use when doing parts save and load
//
class TOcSaveLoad {
  public:
    TOcSaveLoad()
      : StorageI(0), SameAsLoad(false), Remember(true), SaveSelection(false),
        UserData(0) {}
    TOcSaveLoad(IStorage* storageI, bool sameAsLoad = true, bool remember = false,
                bool saveSelection = false, void* userData = 0)
    {
      StorageI      = storageI;
      SameAsLoad    = sameAsLoad;
      Remember      = remember;
      SaveSelection = saveSelection;
      UserData      = userData;
    }

  public:
    IStorage far* StorageI;
    bool          SameAsLoad;      // Same IStorage as one used for loading
    bool          Remember;        // whether to keep the IStorage ptr after save/load
    bool          SaveSelection;   // whether to save only the selection
    void*         UserData;        // User data
};

//
//
//
struct TOcToolBarInfo {
  bool  Show;        // whether this is a show or a hide
  HWND  HFrame;      // Container frame for show, Server frame for hide
  HWND  HLeftTB;
  HWND  HTopTB;
  HWND  HRightTB;
  HWND  HBottomTB;
};

//
// Used to obtain the size of the rectangle that encloses the selection
//
class TOcPartSize {
  public:
    TOcPartSize()
    :
      PartRect(0, 0, 0, 0),
      Selection(false),
      Moniker(0),
      UserData(0)
    {
    }

    TOcPartSize(TRect rect, bool selection = false, TString* moniker = 0, void* userData = 0)
    :
      PartRect(rect),
      Selection(selection),
      Moniker(moniker),
      UserData(userData)
    {
    }

    TOcPartSize(bool selection, TString* moniker = 0, void* userData = 0)
    :
      PartRect(0, 0, 0, 0),
      Selection(selection),
      Moniker(moniker),
      UserData(userData)
    {
    }

  public:
    TRect    PartRect;     // rect enclosing the whole/part of the embedded object
    bool     Selection;    // whether we want rect for the whole or part(selection)
    TString* Moniker;      // we want the selection rect for moniker
    void*    UserData;     // User data
};

//
// Used to obtain the item name for building monikers
//
class TOcItemName {
  public:
    TOcItemName() : Selection(false) {}
    TOcItemName(bool selection) : Selection(selection) {}

  public:
    TString Name;        // Item moniker
    bool    Selection;   // Whether we want name for the whole or part(selection)
};

//
// Used to obtain the native clipboard format data
//
class TOcFormatData {
  public:
    TOcFormatData(TOcFormat& format, void* userData = 0, HANDLE handle = 0,
                  bool paste = false, TPoint far* where = 0)
    :
      Format(format), UserData(userData), Handle(handle),
      Paste(paste), Where(where) {}

  public:
    TOcFormat&  Format;        // Clipboard format
    void*       UserData;      // User data for mapping to a selection
    HANDLE      Handle;        // clipboard data in handle
    bool        Paste;         // are we doing a paste?
    TPoint far* Where;         // Where to drop this data
};

//
// Used to obtain the item name for building monikers
//
class TOcItemLink {
  public:
    TOcItemLink(const char far* moniker, TOcLinkView* view)
    :
      Moniker(moniker),
      OcLinkView(view)
    {
    }

  public:
    const char far*  Moniker;      // item moniker
    TOcLinkView*     OcLinkView;   // view associated with the link
};

//
// class TOcScaleFactor
// ~~~~~ ~~~~~~~~~~~~~~
class TOcScaleFactor {
  public:
    TOcScaleFactor();
    TOcScaleFactor(const TRect& siteRect, const TSize& partSize);
    TOcScaleFactor(const TOcScaleInfo far& scaleInfo);

    TOcScaleFactor& operator =(const TOcScaleInfo far& scaleInfo);
    TOcScaleFactor& operator =(const TOcScaleFactor& scaleFactor);
    void GetScaleFactor(TOcScaleInfo far& scaleInfo) const;

    bool IsZoomed();
    void SetScale(uint16 percent);
    uint16 GetScale();
    void Reset();

  public:
    TSize        SiteSize;
    TSize        PartSize;
};

//
// class TOcSiteRect
// ~~~~~ ~~~~~~~~~~~
class TOcSiteRect {
  public:
    TOcSiteRect();
    TOcSiteRect(TOcPart *part, const TRect& rect);
    TRect     Rect;
    TOcPart*  Part;
};

//----------------------------------------------------------------------------

//
// class TOcContainerHost
// ~~~~~ ~~~~~~~~~~~~~~~~
class _OCMCLASS TOcContainerHost {
  public:
    // OC-Host support
    //
    virtual void  ReleaseOcObject() = 0;
  
    // Container methods
    //
    virtual bool   EvOcViewOpenDoc(const char far* path) = 0;
    virtual const char far* EvOcViewTitle() = 0;
    virtual void   EvOcViewSetTitle(const char far* title) = 0;

    virtual bool   EvOcViewBorderSpaceReq(const TRect far* rect) = 0;
    virtual bool   EvOcViewBorderSpaceSet(const TRect far* rect) = 0;
    virtual bool   EvOcViewDrag(TOcDragDrop far& ddInfo) = 0;
    virtual bool   EvOcViewDrop(TOcDragDrop far& ddInfo) = 0;
    virtual bool   EvOcViewScroll(TOcScrollDir scrollDir) = 0;
    virtual bool   EvOcPartInvalid(TOcPartChangeInfo& part) = 0;
    virtual bool   EvOcViewPasteObject(TOcInitInfo& init) = 0;

    // From site (TOcPart)
    //
//    virtual bool   EvOcViewGetSiteRect(TRect far* rect) = 0;
//    virtual bool   EvOcViewSetSiteRect(TRect far* rect) = 0;
    virtual bool   EvOcViewGetSiteRect(TOcSiteRect& sr) = 0;
    virtual bool   EvOcViewSetSiteRect(TOcSiteRect& sr) = 0;
    virtual bool   EvOcViewPartActivate(TOcPart& ocPart) = 0;
    virtual bool   EvOcViewGetScale(TOcScaleFactor& scaleFactor) = 0;

    virtual HWND   EvOcGetWindow() const = 0;

    // For compatibility, don't need to implement
    //
    virtual void  SetWindow(HWND) {}
};

/*
//
// class TOcCtrlContainerHost
// ~~~~~ ~~~~~~~~~~~~~~~~~~~~
class _OCMCLASS TOcCtrlContainerHost {
  public:
    // OC-Host support
    //
    virtual void  ReleaseOcObject() = 0;
  
    // Control methods
    //
    virtual bool   EvOcViewTransformCoords(uint verb) = 0;

    // Ambient property methods
    //
    virtual bool   EvOcAmbientGetBackColor(long* rgb) = 0;
    virtual bool   EvOcAmbientGetForeColor(long* rgb) = 0;
    virtual bool   EvOcAmbientGetLocaleID(long* locale) = 0;
    virtual bool   EvOcAmbientGetTextAlign(short* align) = 0;
    virtual bool   EvOcAmbientGetMessageReflect(bool* msgReflect) = 0;
    virtual bool   EvOcAmbientGetUserMode(bool* mode) = 0;
    virtual bool   EvOcAmbientGetUIDead(bool* dead) = 0;
    virtual bool   EvOcAmbientGetShowGrabHandles(bool* show) = 0;
    virtual bool   EvOcAmbientGetShowHatching(bool* show) = 0;
    virtual bool   EvOcAmbientGetSupportsMnemonics(bool* support) = 0;
    virtual bool   EvOcAmbientGetDisplayName(TString** name) = 0;
    virtual bool   EvOcAmbientGetScaleUnits(TString** units) = 0;
    virtual bool   EvOcAmbientGetFont(IDispatch** font) = 0;

    virtual bool   EvOcAmbientSetBackColor(long rgb) = 0;
    virtual bool   EvOcAmbientSetForeColor(long rgb) = 0;
    virtual bool   EvOcAmbientSetLocaleID(long locale) = 0;
    virtual bool   EvOcAmbientSetTextAlign(short align) = 0;
    virtual bool   EvOcAmbientSetMessageReflect(bool msgReflect) = 0;
    virtual bool   EvOcAmbientSetUserMode(bool mode) = 0;
    virtual bool   EvOcAmbientSetUIDead(bool dead) = 0;
    virtual bool   EvOcAmbientSetShowGrabHandles(bool show) = 0;
    virtual bool   EvOcAmbientSetShowHatching(bool show) = 0;
    virtual bool   EvOcAmbientSetSupportsMnemonics(bool support) = 0;
    virtual bool   EvOcAmbientSetDisplayName(TString* name) = 0;
    virtual bool   EvOcAmbientSetScaleUnits(TString* units) = 0;
    virtual bool   EvOcAmbientSetFont(IDispatch* font) = 0;

    // Ctrl event methods
    //
    virtual bool   EvOcCtrlClick(TCtrlEvent* pev) = 0;
    virtual bool   EvOcCtrlDblClick(TCtrlEvent* pev) = 0;
    virtual bool   EvOcCtrlMouseDown(TCtrlMouseEvent* pev) = 0;
    virtual bool   EvOcCtrlMouseMove(TCtrlMouseEvent* pev) = 0;
    virtual bool   EvOcCtrlMouseUp(TCtrlMouseEvent* pev) = 0;
    virtual bool   EvOcCtrlKeyDown(TCtrlKeyEvent* pev) = 0;
    virtual bool   EvOcCtrlKeyUp(TCtrlKeyEvent* pev) = 0;
    virtual bool   EvOcCtrlErrorEvent(TCtrlErrorEvent* pev) = 0;
    virtual bool   EvOcCtrlFocus(TCtrlFocusEvent* pev) = 0;
    virtual bool   EvOcCtrlPropertyChange(TCtrlPropertyEvent* pev) = 0;
    virtual bool   EvOcCtrlPropertyRequestEdit(TCtrlPropertyEvent* pev) = 0;
    virtual bool   EvOcCtrlCustomEvent(TCtrlCustomEvent* pev) = 0;
};
*/

//
// class TOcServerHost
// ~~~~~ ~~~~~~~~~~~~~
class _OCMCLASS TOcServerHost {
  public:
    // OC-Host support
    //
    virtual void  ReleaseOcObject() = 0;
  
    // Data server methods
    //
    virtual bool   EvOcViewGetItemName(TOcItemName& item) = 0;
    virtual bool   EvOcViewClipData(TOcFormatData far& format) = 0;
    virtual bool   EvOcViewSetData(TOcFormatData far& format) = 0;
    virtual bool   EvOcViewPartSize(TOcPartSize far& size) = 0;
    virtual bool   EvOcViewSavePart(TOcSaveLoad far& ocSave) = 0;
    virtual bool   EvOcViewPaint(TOcViewPaint far& vp) = 0;

    // Link server methods
    //
    virtual bool   EvOcViewAttachWindow(bool attach) = 0;
    virtual void   EvOcViewSetTitle(const char far* title) = 0; // Open editing
    virtual bool   EvOcViewDrag(TOcDragDrop far& ddInfo) = 0;   // drag from server
    virtual bool   EvOcViewSetLink(TOcLinkView& view) = 0;
    virtual bool   EvOcViewBreakLink(TOcLinkView& view) = 0;
    virtual bool   EvOcViewGetPalette(LOGPALETTE far* far* palette) = 0;

    // Embed server methods
    //
    virtual bool   EvOcViewLoadPart(TOcSaveLoad far& ocLoad) = 0;
    virtual bool   EvOcViewInsMenus(TOcMenuDescr far& sharedMenu) = 0;
    virtual bool   EvOcViewShowTools(TOcToolBarInfo far& tbi) = 0;

    virtual bool   EvOcViewClose() = 0;
    virtual bool   EvOcViewSetScale(TOcScaleFactor& scaleFactor) = 0;
    virtual bool   EvOcViewDoVerb(uint verb) = 0;

    virtual HWND   EvOcGetWindow() const = 0;

    // For compatibility, don't need to implement
    //
    virtual void  SetWindow(HWND) {}
};

//----------------------------------------------------------------------------

//
// class TOcContainerHostMsg
// ~~~~~ ~~~~~~~~~~~~~~~~~~~
class _OCMCLASS TOcContainerHostMsg : public TOcContainerHost {
  public:
    TOcContainerHostMsg() : Wnd(0) {}

    // OC-Host support
    //
    virtual void  ReleaseOcObject() {delete this;}
  
    // Container methods
    //
    virtual bool   EvOcViewOpenDoc(const char far* path);
    virtual const char far* EvOcViewTitle();
    virtual void   EvOcViewSetTitle(const char far* title);

    virtual bool   EvOcViewBorderSpaceReq(const TRect far* rect);
    virtual bool   EvOcViewBorderSpaceSet(const TRect far* rect);
    virtual bool   EvOcViewDrag(TOcDragDrop far& ddInfo);
    virtual bool   EvOcViewDrop(TOcDragDrop far& ddInfo);
    virtual bool   EvOcViewScroll(TOcScrollDir scrollDir);
    virtual bool   EvOcPartInvalid(TOcPartChangeInfo& part);
    virtual bool   EvOcViewPasteObject(TOcInitInfo& init);

    // From site (TOcPart)
    //
//    virtual bool   EvOcViewGetSiteRect(TRect far* rect);
//    virtual bool   EvOcViewSetSiteRect(TRect far* rect);
    virtual bool   EvOcViewGetSiteRect(TOcSiteRect& sr);
    virtual bool   EvOcViewSetSiteRect(TOcSiteRect& sr);
    virtual bool   EvOcViewPartActivate(TOcPart& ocPart);
    virtual bool   EvOcViewGetScale(TOcScaleFactor& scaleFactor);

    virtual HWND   EvOcGetWindow() const {return Wnd;}

    // For compatibility
    //
    virtual void  SetWindow(HWND hWnd) {Wnd = hWnd;}

  protected:
    uint32 ForwardEvent(int eventId, const void far* param);
    uint32 ForwardEvent(int eventId, uint32 param = 0);

  private:
    HWND   Wnd;  // For sending messages to real host window, & window ops

  friend TOcView;  
};

//
// class TOcServerHostMsg
// ~~~~~ ~~~~~~~~~~~~~~~~
class _OCMCLASS TOcServerHostMsg : public TOcServerHost {
  public:
    TOcServerHostMsg() : Wnd(0) {}

    // OC-Host support
    //
    virtual void  ReleaseOcObject() {delete this;}
  
    // Data server methods
    //
    virtual bool   EvOcViewGetItemName(TOcItemName& item);
    virtual bool   EvOcViewClipData(TOcFormatData far& format);
    virtual bool   EvOcViewSetData(TOcFormatData far& format);
    virtual bool   EvOcViewPartSize(TOcPartSize far& size);
    virtual bool   EvOcViewSavePart(TOcSaveLoad far& ocSave);
    virtual bool   EvOcViewPaint(TOcViewPaint far& vp);

    // Link server methods
    //
    virtual bool   EvOcViewAttachWindow(bool attach);
    virtual void   EvOcViewSetTitle(const char far* title); // Open editing
    virtual bool   EvOcViewDrag(TOcDragDrop far& ddInfo);   // drag from server
    virtual bool   EvOcViewSetLink(TOcLinkView& view);
    virtual bool   EvOcViewBreakLink(TOcLinkView& view);
    virtual bool   EvOcViewGetPalette(LOGPALETTE far* far* palette);

    // Embed server methods
    //
    virtual bool   EvOcViewLoadPart(TOcSaveLoad far& ocLoad);
    virtual bool   EvOcViewInsMenus(TOcMenuDescr far& sharedMenu);
    virtual bool   EvOcViewShowTools(TOcToolBarInfo far& tbi);

    virtual bool   EvOcViewClose();
    virtual bool   EvOcViewSetScale(TOcScaleFactor& scaleFactor);
    virtual bool   EvOcViewDoVerb(uint verb);

    virtual HWND   EvOcGetWindow() const {return Wnd;}

    // For compatibility
    //
    virtual void  SetWindow(HWND hWnd) {Wnd = hWnd;}

  protected:
    uint32 ForwardEvent(int eventId, const void far* param);
    uint32 ForwardEvent(int eventId, uint32 param = 0);

  private:
    HWND   Wnd;  // For sending messages to real host window
};

//
inline uint32
TOcView::ForwardEvent(int eventId, const void far* param)
{
  return dynamic_cast<TOcContainerHostMsg*>(ContainerHost)->
           ForwardEvent(eventId, param);
}

inline uint32
TOcView::ForwardEvent(int eventId, uint32 param)
{
  return dynamic_cast<TOcContainerHostMsg*>(ContainerHost)->
           ForwardEvent(eventId, param);
}


//----------------------------------------------------------------------------
// Subdispatch IDs for TOcView clients
//
                                    // Container
#define OC_VIEWOPENDOC        0x0200  // ask container to open itself
#define OC_VIEWTITLE          0x0201  // Get view window title
#define OC_VIEWSETTITLE       0x0202  // Set view window title
#define OC_VIEWBORDERSPACEREQ 0x0203  // Request/Set border space in view
#define OC_VIEWBORDERSPACESET 0x0204  // Request/Set border space in view
#define OC_VIEWDRAG           0x0205  // Provide drag feedback (c&s)
#define OC_VIEWDROP           0x0206  // Accept a drop
#define OC_VIEWSCROLL         0x0207  // Scroll view
#define OC_VIEWPARTINVALID    0x0208  // a part needs repainting
#define OC_VIEWPASTEOBJECT    0x0209  // let container know a part is pasted

#define OC_VIEWGETSITERECT    0x020A  // Ask container for the site rect
#define OC_VIEWSETSITERECT    0x020B  // Ask container to set the site rect
#define OC_VIEWPARTACTIVATE   0x020C  // Let container know the part is activated
#define OC_VIEWGETSCALE       0x020D  // Ask container to give scaling info

                                    // Data Server
#define OC_VIEWGETITEMNAME    0x0301  // Ask container to name its content or selection
#define OC_VIEWCLIPDATA       0x0302  // Ask for a clip data
#define OC_VIEWSETDATA        0x0303  // Set format data into server
#define OC_VIEWPARTSIZE       0x0304  // Ask server for a its extent
#define OC_VIEWSAVEPART       0x0305  // Ask server to save document
#define OC_VIEWPAINT          0x0306  // Paint remote view. Like WM_PAINT + extra

                                    // Link Server
#define OC_VIEWATTACHWINDOW   0x0310  // RL: ask server to attach to its owner window
#define OC_VIEWSETLINK        0x0311  // Ask to establish link to item
#define OC_VIEWBREAKLINK      0x0312  // Ask server to break link to item
#define OC_VIEWGETPALETTE     0x0313  // RL: ask server for its logical palette

                                    // Embed Server
#define OC_VIEWLOADPART       0x0321  // R: ask server to load document
#define OC_VIEWINSMENUS       0x0322  // R: ask server to insert its menus in mbar
#define OC_VIEWSHOWTOOLS      0x0323  // R: ask server to show its tools
#define OC_VIEWCLOSE          0x0324  // R: tell server to close this remote view
#define OC_VIEWSETSCALE       0x0325  // R: ask server to handle scaling
#define OC_VIEWDOVERB         0x0326  // R: ask server to do a verb

// Control event notification from control part to container view
//
#define OC_VIEWTRANSFORMCOORDS         0x0330 // C: view to transform ctrl coords
#define OC_CTRLEVENT_FOCUS             0x0340  // Ctrl got/lost focus notify
#define OC_CTRLEVENT_PROPERTYCHANGE    0x0341  // Ctrl prop changed notify
#define OC_CTRLEVENT_PROPERTYREQUESTEDIT 0x0342// Ctrl prop request edit
#define OC_CTRLEVENT_CLICK             0x0343  // Ctrl click notify
#define OC_CTRLEVENT_DBLCLICK          0x0344  // Ctrl dblclick notify
#define OC_CTRLEVENT_MOUSEDOWN         0x0345  // Ctrl mouse down notify
#define OC_CTRLEVENT_MOUSEMOVE         0x0346  // Ctrl mouse move notify
#define OC_CTRLEVENT_MOUSEUP           0x0347  // Ctrl mouse up notify
#define OC_CTRLEVENT_KEYDOWN           0x0348  // Ctrl key down notify
#define OC_CTRLEVENT_KEYUP             0x0349  // Ctrl key up notify
#define OC_CTRLEVENT_ERROREVENT        0x034A  // Ctrl error event notify
#define OC_CTRLEVENT_CUSTOMEVENT       0x034B  // Ctrl custom event notify

// Control request for view ambient properties from container view
//
#define OC_AMBIENT_GETBACKCOLOR        0x0350
#define OC_AMBIENT_GETFORECOLOR        0x0351
#define OC_AMBIENT_GETSHOWGRABHANDLES  0x0352
#define OC_AMBIENT_GETUIDEAD           0x0353
#define OC_AMBIENT_GETSUPPORTSMNEMONICS 0x0354
#define OC_AMBIENT_GETSHOWHATCHING     0x0355
#define OC_AMBIENT_GETDISPLAYASDEFAULT 0x0356
#define OC_AMBIENT_GETTEXTALIGN        0x0357
#define OC_AMBIENT_GETMESSAGEREFLECT   0x0358
#define OC_AMBIENT_GETLOCALEID         0x0359
#define OC_AMBIENT_GETUSERMODE         0x035A
#define OC_AMBIENT_GETDISPLAYNAME      0x035B
#define OC_AMBIENT_GETSCALEUNITS       0x035C
#define OC_AMBIENT_GETFONT             0x035D

// Control set view ambient properties
//
#define OC_AMBIENT_SETBACKCOLOR        0x0360
#define OC_AMBIENT_SETFORECOLOR        0x0361
#define OC_AMBIENT_SETSHOWGRABHANDLES  0x0362
#define OC_AMBIENT_SETUIDEAD           0x0363
#define OC_AMBIENT_SETSUPPORTSMNEMONICS 0x0364
#define OC_AMBIENT_SETSHOWHATCHING     0x0365
#define OC_AMBIENT_SETDISPLAYASDEFAULT 0x0366
#define OC_AMBIENT_SETTEXTALIGN        0x0367
#define OC_AMBIENT_SETMESSAGEREFLECT   0x0368
#define OC_AMBIENT_SETLOCALEID         0x0369
#define OC_AMBIENT_SETUSERMODE         0x036A
#define OC_AMBIENT_SETDISPLAYNAME      0x036B
#define OC_AMBIENT_SETSCALEUNITS       0x036C
#define OC_AMBIENT_SETFONT             0x036D

#define OC_USEREVENT                   0xF000  // User defined events

//----------------------------------------------------------------------------
// Inline Implementations
//

//
inline bool TOcView::IsOptionSet(uint32 option) const
{
  return ToBool(Options & option);
}

//
inline void TOcView::SetOption(uint32 bit, bool state)
{
  if (state)
    Options |= bit;
  else
    Options &= ~bit;
}

#endif  // OCF_OCVIEW_H

