function point() {
    var pt = new Point(10, 15);

    pt.SetX(20);
    pt.SetY(30);

    return new Array(pt.GetX(), pt.GetY());
}

point();