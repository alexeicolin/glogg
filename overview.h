/*
 * Copyright (C) 2011, 2012 Nicolas Bonnefon and other contributors
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

#ifndef OVERVIEW_H
#define OVERVIEW_H

#include <QList>

#include "logfiltereddata.h"

// Class implementing the logic behind the matches overview bar.
// This class converts the matches found in a LogFilteredData in
// a screen dependent set of coloured lines, which is cached.
// This class is not a UI class, actual display is left to the client.
//
// This class is NOT thread-safe.
class Overview
{
  public:
    // A line with a position in pixel and a weight (darkness)
    class WeightedLine {
      public:
        static const int WEIGHT_STEPS = 3;

        WeightedLine() { pos_ = 0; weight_ = 0; }
        // (Necessary for QVector)
        WeightedLine( int pos ) { pos_ = pos; weight_ = 0; }

        int position() const { return pos_; }
        int weight() const { return weight_; }

        void load() { weight_ = qMin( weight_ + 1, WEIGHT_STEPS - 1 ); }

      private:
        int pos_;
        int weight_;
    };

    Overview();
    ~Overview();

    // Associate the passed filteredData to this Overview
    void setFilteredData( const LogFilteredData* logFilteredData );
    // Signal the overview its attached LogFilteredData has been changed and
    // the overview must be updated with the provided total number
    // of line of the file.
    void updateData( int totalNbLine );
    // Set the visibility flag of this overview.
    void setVisible( bool visible ) { visible_ = visible; dirty_ = visible; }

    // Update the current position in the file (to draw the view line)
    void updateCurrentPosition( int firstLine, int lastLine )
    { topLine_ = firstLine; nbLines_ = lastLine - firstLine; }

    // Returns weither this overview is visible.
    bool isVisible() { return visible_; }
    // Signal the overview the height of the display has changed, triggering
    // an update of its cache.
    void updateView( int height );
    // Returns a list of lines (between 0 and 'height') representing matches.
    // (pointer returned is valid until next call to update*()
    const QVector<WeightedLine>* getMatchLines() const;
    // Returns a list of lines (between 0 and 'height') representing marks.
    // (pointer returned is valid until next call to update*()
    const QVector<WeightedLine>* getMarkLines() const;
    // Return a pair of lines (between 0 and 'height') representing the current view.
    std::pair<int,int> getViewLines() const;

    // Return the line number corresponding to the passed overview y coordinate.
    int fileLineFromY( int y ) const;

  private:
    // List of matches associated with this Overview.
    const LogFilteredData* logFilteredData_;
    // Total number of lines in the file.
    int linesInFile_;
    // Whether the overview is visible.
    bool visible_;
    // First and last line currently viewed.
    int topLine_;
    int nbLines_;
    // Current height of view window.
    int height_;
    // Does the cache (matchesLines, markLines) need to be recalculated.
    int dirty_;

    // List of lines representing matches and marks (are shared with the client)
    QVector<WeightedLine> matchLines_;
    QVector<WeightedLine> markLines_;

    void recalculatesLines();
};

#endif
