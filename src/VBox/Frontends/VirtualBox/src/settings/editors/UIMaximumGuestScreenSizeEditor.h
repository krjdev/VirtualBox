/* $Id: UIMaximumGuestScreenSizeEditor.h 94057 2022-03-02 14:14:37Z vboxsync $ */
/** @file
 * VBox Qt GUI - UIMaximumGuestScreenSizeEditor class declaration.
 */

/*
 * Copyright (C) 2019-2022 Oracle Corporation
 *
 * This file is part of VirtualBox Open Source Edition (OSE), as
 * available from http://www.virtualbox.org. This file is free software;
 * you can redistribute it and/or modify it under the terms of the GNU
 * General Public License (GPL) as published by the Free Software
 * Foundation, in version 2 as it comes in the "COPYING" file of the
 * VirtualBox OSE distribution. VirtualBox OSE is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY of any kind.
 */

#ifndef FEQT_INCLUDED_SRC_settings_editors_UIMaximumGuestScreenSizeEditor_h
#define FEQT_INCLUDED_SRC_settings_editors_UIMaximumGuestScreenSizeEditor_h
#ifndef RT_WITHOUT_PRAGMA_ONCE
# pragma once
#endif

/* Qt includes: */
#include <QWidget>

/* GUI includes: */
#include "QIWithRetranslateUI.h"
#include "UIExtraDataDefs.h"
#include "UILibraryDefs.h"

/* Forward declarations: */
class QGridLayout;
class QLabel;
class QSpinBox;
class QComboBox;


/** Maximum guest screen size value. */
struct SHARED_LIBRARY_STUFF UIMaximumGuestScreenSizeValue
{
    /** Constructs maximum guest screen size value. */
    UIMaximumGuestScreenSizeValue(MaximumGuestScreenSizePolicy enmPolicy = MaximumGuestScreenSizePolicy_Any,
                                  const QSize &resolution = QSize());

    /** Returns whether the @a other passed data is equal to this one. */
    bool equal(const UIMaximumGuestScreenSizeValue &other) const;
    /** Returns whether the @a other passed data is equal to this one. */
    bool operator==(const UIMaximumGuestScreenSizeValue &other) const { return equal(other); }
    /** Returns whether the @a other passed data is different from this one. */
    bool operator!=(const UIMaximumGuestScreenSizeValue &other) const { return !equal(other); }

    /** Holds the maximum guest-screen policy. */
    MaximumGuestScreenSizePolicy  m_enmPolicy;
    /** Holds the maximum guest-screen size. */
    QSize                         m_size;
};
Q_DECLARE_METATYPE(UIMaximumGuestScreenSizeValue);


/** QWidget subclass used as a maximum guest screen size editor. */
class SHARED_LIBRARY_STUFF UIMaximumGuestScreenSizeEditor : public QIWithRetranslateUI<QWidget>
{
    Q_OBJECT;

signals:

    /** Notifies listeners about @a guiValue change. */
    void sigValueChanged(const UIMaximumGuestScreenSizeValue &guiValue);

public:

    /** Constructs maximum guest screen size editor passing @a pParent to the base-class. */
    UIMaximumGuestScreenSizeEditor(QWidget *pParent = 0);

    /** Returns focus proxy 1. */
    QWidget *focusProxy1() const;
    /** Returns focus proxy 2. */
    QWidget *focusProxy2() const;
    /** Returns focus proxy 3. */
    QWidget *focusProxy3() const;

    /** Defines editor @a guiValue. */
    void setValue(const UIMaximumGuestScreenSizeValue &guiValue);
    /** Returns editor value. */
    UIMaximumGuestScreenSizeValue value() const;

    /** Returns minimum layout hint. */
    int minimumLabelHorizontalHint() const;
    /** Defines minimum layout @a iIndent. */
    void setMinimumLayoutIndent(int iIndent);

protected:

    /** Handles translation event. */
    virtual void retranslateUi() RT_OVERRIDE;

private slots:

    /** Handles current policy index change. */
    void sltHandleCurrentPolicyIndexChanged();
    /** Handles size change. */
    void sltHandleSizeChanged();

private:

    /** Prepares all. */
    void prepare();
    /** Populates combo. */
    void populateCombo();

    /** Holds the value to be selected. */
    UIMaximumGuestScreenSizeValue  m_guiValue;

    /** Holds the main layout: */
    QGridLayout *m_pLayout;
    /** Holds the policy label instance. */
    QLabel      *m_pLabelPolicy;
    /** Holds the policy combo instance. */
    QComboBox   *m_pComboPolicy;
    /** Holds the max width label instance. */
    QLabel      *m_pLabelMaxWidth;
    /** Holds the max width spinbox instance. */
    QSpinBox    *m_pSpinboxMaxWidth;
    /** Holds the max height label instance. */
    QLabel      *m_pLabelMaxHeight;
    /** Holds the max height spinbox instance. */
    QSpinBox    *m_pSpinboxMaxHeight;
};


#endif /* !FEQT_INCLUDED_SRC_settings_editors_UIMaximumGuestScreenSizeEditor_h */
