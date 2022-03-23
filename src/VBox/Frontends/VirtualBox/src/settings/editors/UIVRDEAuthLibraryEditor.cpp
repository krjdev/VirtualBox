/* $Id: UIVRDEAuthLibraryEditor.cpp 93935 2022-02-24 16:40:30Z vboxsync $ */
/** @file
 * VBox Qt GUI - UIVRDEAuthLibraryEditor class implementation.
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

/* Qt includes: */
#include <QGridLayout>
#include <QLabel>

/* GUI includes: */
#include "UICommon.h"
#include "UIFilePathSelector.h"
#include "UIVRDEAuthLibraryEditor.h"


UIVRDEAuthLibraryEditor::UIVRDEAuthLibraryEditor(QWidget *pParent /* = 0 */)
    : QIWithRetranslateUI<QWidget>(pParent)
    , m_strValue(QString())
    , m_pLabel(0)
    , m_pSelector(0)
{
    prepare();
}

void UIVRDEAuthLibraryEditor::setValue(const QString &strValue)
{
    if (m_pSelector)
    {
        /* Update cached value and editor
         * if value has changed: */
        if (m_strValue != strValue)
        {
            m_strValue = strValue;
            m_pSelector->setPath(strValue);
        }
    }
}

QString UIVRDEAuthLibraryEditor::value() const
{
    return m_pSelector ? m_pSelector->path() : m_strValue;
}

int UIVRDEAuthLibraryEditor::minimumLabelHorizontalHint() const
{
    return m_pLabel->minimumSizeHint().width();
}

void UIVRDEAuthLibraryEditor::setMinimumLayoutIndent(int iIndent)
{
    if (m_pLayout)
        m_pLayout->setColumnMinimumWidth(0, iIndent);
}

void UIVRDEAuthLibraryEditor::retranslateUi()
{
    if (m_pLabel)
        m_pLabel->setText(tr("V&RDP Authentication Library:"));
    if (m_pSelector)
        m_pSelector->setToolTip(tr("Holds the path to the library that provides "
                                   "authentication for Remote Display (VRDP) clients."));
}

void UIVRDEAuthLibraryEditor::sltHandleSelectorPathChanged()
{
    if (m_pSelector)
        emit sigValueChanged(m_pSelector->path());
}

void UIVRDEAuthLibraryEditor::prepare()
{
    /* Create main layout: */
    m_pLayout = new QGridLayout(this);
    if (m_pLayout)
    {
        m_pLayout->setContentsMargins(0, 0, 0, 0);
        m_pLayout->setColumnStretch(1, 1);

        /* Create label: */
        m_pLabel = new QLabel(this);
        if (m_pLabel)
        {
            m_pLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
            m_pLayout->addWidget(m_pLabel, 0, 0);
        }

        /* Create selector: */
        m_pSelector = new UIFilePathSelector(this);
        if (m_pSelector)
        {
            if (m_pLabel)
                m_pLabel->setBuddy(m_pSelector);
            m_pSelector->setInitialPath(uiCommon().homeFolder());
            m_pSelector->setMode(UIFilePathSelector::Mode_File_Open);
            connect(m_pSelector, &UIFilePathSelector::pathChanged,
                    this, &UIVRDEAuthLibraryEditor::sltHandleSelectorPathChanged);

            m_pLayout->addWidget(m_pSelector, 0, 1);
        }
    }

    /* Apply language settings: */
    retranslateUi();
}
