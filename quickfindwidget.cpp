/*
 * Copyright (C) 2010 Nicolas Bonnefon and other contributors
 *
 * This file is part of glogg.
 *
 * glogg is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * glogg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with glogg.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "log.h"

#include "configuration.h"

#include "quickfindwidget.h"

const int QuickFindWidget::NOTIFICATION_TIMEOUT = 5000;

QuickFindWidget::QuickFindWidget( QWidget* parent ) : QWidget( parent )
{
    // ui_.setupUi( this );
    // setFocusProxy(ui_.findEdit);
    // setProperty("topBorder", true);
    QHBoxLayout *layout = new QHBoxLayout( this );

    layout->setMargin( 0 );
    layout->setSpacing( 6 );

    closeButton_ = setupToolButton(
            QLatin1String(""), QLatin1String( ":/images/darkclosebutton.png" ) );
    layout->addWidget( closeButton_ );
    connect( closeButton_, SIGNAL( clicked() ), SLOT( closeHandler() ) );

    editQuickFind_ = new QLineEdit( this );
    // FIXME: set MinimumSize might be to constraining
    editQuickFind_->setMinimumSize( QSize( 150, 0 ) );
    layout->addWidget( editQuickFind_ );
    /*
    connect( editQuickFind_. SIGNAL( textChanged( QString ) ), this,
            SLOT( textChanged( QString ) ) );
    connect( editQuickFind_. SIGNAL( textChanged( QString ) ), this,
            SLOT( updateButtons() ) );
    */
    connect( editQuickFind_, SIGNAL( returnPressed() ),
            this, SLOT( returnHandler() ) );
    previousButton_ = setupToolButton( QLatin1String("Previous"),
            QLatin1String( ":/images/arrowup.png" ) );
    layout->addWidget( previousButton_ );
    connect( previousButton_, SIGNAL( clicked() ),
            this, SLOT( doSearchBackward() ) );

    nextButton_ = setupToolButton( QLatin1String("Next"),
            QLatin1String( ":/images/arrowdown.png" ) );
    layout->addWidget( nextButton_ );
    connect( nextButton_, SIGNAL( clicked() ),
            this, SLOT( doSearchForward() ) );

    notificationText_ = new QLabel( "" );
    // FIXME: set MinimumSize might be to constraining
    notificationText_->setMinimumSize( QSize( 150, 0 ) );
    layout->addWidget( notificationText_ );

#if 0
    QSpacerItem* spacerItem = new QSpacerItem(20, 20, QSizePolicy::Expanding,
        QSizePolicy::Minimum);
    layout_->addItem(spacerItem);
#endif
    setMinimumWidth( minimumSizeHint().width() );

    // Notification timer:
    notificationTimer_ = new QTimer( this );
    notificationTimer_->setSingleShot( true );
    connect( notificationTimer_, SIGNAL( timeout() ),
            this, SLOT( notificationTimeout() ) );

}

void QuickFindWidget::userActivate( QFDirection direction )
{
    direction_ = direction;
    userRequested_ = true;
    QWidget::show();
    editQuickFind_->setFocus( Qt::ShortcutFocusReason );
}

//
// SLOTS
//

void QuickFindWidget::changeDisplayedPattern( const QString& newPattern )
{
    editQuickFind_->setText( newPattern );
}

void QuickFindWidget::notify( const QString& message )
{
    LOG(logDEBUG) << "QuickFindWidget::notify()";

    notificationText_->setText( message );
    QWidget::show();
    notificationTimer_->start( NOTIFICATION_TIMEOUT );
}

void QuickFindWidget::clearNotification()
{
    LOG(logDEBUG) << "QuickFindWidget::clearNotification()";

    notificationText_->setText( "" );
}

// User clicks forward arrow
void QuickFindWidget::doSearchForward()
{
    LOG(logDEBUG) << "QuickFindWidget::doSearchForward()";

    // The user has clicked on a button, so we assume she wants
    // the widget to stay visible.
    userRequested_ = true;

    emit patternConfirmed( editQuickFind_->text() );
    emit searchForward();
}

// User clicks backward arrow
void QuickFindWidget::doSearchBackward()
{
    LOG(logDEBUG) << "QuickFindWidget::doSearchBackward()";

    // The user has clicked on a button, so we assume she wants
    // the widget to stay visible.
    userRequested_ = true;

    emit patternConfirmed( editQuickFind_->text() );
    emit searchBackward();
}

// Close and search when the user presses Return
void QuickFindWidget::returnHandler()
{
    emit patternConfirmed( editQuickFind_->text() );
    // Close the widget
    userRequested_ = false;
    this->hide();
    emit close();
    emit searchForward();
}

// Close and reset flag when the user clicks 'close'
void QuickFindWidget::closeHandler()
{
    userRequested_ = false;
    this->hide();
    emit close();
}

void QuickFindWidget::notificationTimeout()
{
    // We close the widget if the user hasn't explicitely requested it.
    if ( userRequested_ == false )
        this->hide();
}

//
// Private functions
//
QToolButton* QuickFindWidget::setupToolButton(const QString &text, const QString &icon)
{
    QToolButton *toolButton = new QToolButton(this);

    toolButton->setText(text);
    toolButton->setAutoRaise(true);
    toolButton->setIcon(QIcon(icon));
    toolButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    return toolButton;
}
