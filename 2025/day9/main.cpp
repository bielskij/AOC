#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"

struct Rect {
    Point<int64_t> c1;
    Point<int64_t> c2;

    mutable int64_t _area = -1;

    Rect (const Point<int64_t> &c1, const Point<int64_t> &c2) : c1(c1), c2(c2) {
    }

    int64_t area() const {
        if (_area == -1) {
            _area = (std::abs(c1.x() - c2.x()) + 1) * (std::abs(c1.y() - c2.y()) + 1);
        }

        return _area;
    }
};

struct Edge {
    Point<int64_t> c1;
    Point<int64_t> c2;

    Edge(const Point<int64_t> &start, const Point<int64_t> &end) {
        if (start.x() == end.x()) {
            c1.set(start.x(), std::min(start.y(), end.y()));
            c2.set(start.x(), std::max(start.y(), end.y()));

        } else if (start.y() == end.y()) {
            c1.set(std::min(start.x(), end.x()), start.y());
            c2.set(std::max(start.x(), end.x()), start.y());

        } else {
            c1 = start;
            c2 = end;
        }
    }
};

int main(int argc, char *argv[]) {
    auto lines = File::readAllLines(argv[1]);

    {
        std::vector<Point<int64_t>> points;

        for (const auto &l : lines) {
            auto toks = utils::strTok(l, ',');

            points.push_back(Point<int64_t>(std::stoi(toks[0]), std::stoi(toks[1])));
        }

        std::vector<Rect> allRects;

        std::vector<Edge> horizontalEdges;
        std::vector<Edge> verticalEdges;

        for (int i = 0; i < points.size(); i++) {
            const auto &srcPoint = points[i];
            const auto &dstPoint = points[(i + 1) % points.size()];

            if (srcPoint.x() == dstPoint.x()) {
                verticalEdges.emplace_back(Edge(srcPoint, dstPoint));

            } else if (srcPoint.y() == dstPoint.y()) {
                horizontalEdges.emplace_back(Edge(srcPoint, dstPoint));
            }
        }

        {
            int64_t maxSize = -1;

            for (int i = 0; i < points.size(); i++) {
                for (int j = i + 1; j < points.size(); j++) {
                    auto newRect = Rect(points[i], points[j]);

                    if (newRect.area() > maxSize) {
                        maxSize = newRect.area();
                    }

                    allRects.emplace_back(newRect);
                }
            }

            PRINTF(("PART_A: %" PRId64, maxSize));
        }

        std::sort(allRects.begin(), allRects.end(), [](const Rect &r1, const Rect &r2) {
            return r1.area() > r2.area();
        });

        for (auto i = 0; i < allRects.size(); i++) {
            auto &rectCandidate = allRects[i];

            auto minV = Point<int64_t>(
                std::min(rectCandidate.c1.x(), rectCandidate.c2.x()), 
                std::min(rectCandidate.c1.y(), rectCandidate.c2.y())
            );
            
            auto maxV = Point<int64_t>(
                std::max(rectCandidate.c1.x(), rectCandidate.c2.x()),
                std::max(rectCandidate.c1.y(), rectCandidate.c2.y())
            );

            bool isValid = true;

            {
                std::vector<Point<int64_t>> toCheck = {
                    Point<int64_t>(minV.x(), minV.y()),
                    Point<int64_t>(minV.x(), maxV.y()),
                    Point<int64_t>(maxV.x(), minV.y()),
                    Point<int64_t>(maxV.x(), maxV.y())
                };

                // Check if point is inside the poligon or on edge
                for (const auto &p : toCheck) {
                    bool onEdge   = false;
                    bool isInside = false;

                    {
                        int crossCount = 0;

                        for (const auto &edge : verticalEdges) {
                            if (((edge.c1.y() > p.y()) ^ (edge.c2.y() > p.y())) && (p.x() < edge.c1.x())) {
                                crossCount++;
                            }
                        }

                        isInside = ((crossCount % 2) == 1);
                    }

                    {
                        for (const auto &edge : verticalEdges) {
                            if (
                                edge.c1.x() == p.x() &&
                                edge.c1.y() <= p.y() &&
                                edge.c2.y() >= p.y()
                            ) {
                                onEdge = true;
                                break;
                            }
                        }

                        if (! onEdge) {
                            for (const auto &edge : horizontalEdges) {
                                if (
                                    edge.c1.y() == p.y() &&
                                    edge.c1.x() <= p.x() &&
                                    edge.c2.x() >= p.x()
                                ) {
                                    onEdge = true;
                                    break;
                                }
                            }
                        }
                    }

                    if (! isInside && ! onEdge) {
                        isValid = false;
                        break;
                    }
                }
            }

            if (! isValid) {
                continue;
            }

            {
                bool anyCross = false;

                // Check for vertical edge cross
                {
                    std::vector<Edge> toCheck = {
                        Edge(Point<int64_t>(minV.x(), minV.y()), Point<int64_t>(maxV.x(), minV.y())),
                        Edge(Point<int64_t>(minV.x(), maxV.y()), Point<int64_t>(maxV.x(), maxV.y()))
                    };

                    for (const auto &edge : verticalEdges) {
                        for (const auto &rectEdge : toCheck) {
                            if (
                                edge.c1.x() > rectEdge.c1.x() && 
                                edge.c2.x() < rectEdge.c2.x() &&
                                edge.c1.y() < rectEdge.c1.y() &&
                                edge.c2.y() > rectEdge.c2.y()
                            ) {
                                anyCross = true;
                                break;
                            }
                        }

                        if (anyCross) {
                            break;
                        }
                    }
                }

                // Check for horizontal edge cross
                if (! anyCross) {
                    std::vector<Edge> toCheck = {
                        Edge(Point<int64_t>(minV.x(), minV.y()), Point<int64_t>(minV.x(), maxV.y())),
                        Edge(Point<int64_t>(maxV.x(), minV.y()), Point<int64_t>(maxV.x(), maxV.y()))
                    };

                    for (const auto &edge : horizontalEdges) {
                        for (const auto &rectEdge : toCheck) {
                            if (
                                edge.c1.y() > rectEdge.c1.y() && 
                                edge.c2.y() < rectEdge.c2.y() &&
                                edge.c1.x() < rectEdge.c1.x() &&
                                edge.c2.x() > rectEdge.c2.x()
                            ) {
                                anyCross = true;
                                break;
                            }
                        }

                        if (anyCross) {
                            break;
                        }
                    }
                }

                isValid = ! anyCross;
            }

            if (isValid) {
                PRINTF(("PART_B: %" PRId64, rectCandidate.area()));
                break;
            }
        }
    }

    return 0;
}
