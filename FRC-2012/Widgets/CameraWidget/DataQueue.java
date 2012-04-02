public class DataQueue {
    /* 'hoopset' holds all results, including outlines of hoops (via
     * regions vector contained within), and estimated 'robotPosition'
     * (as Point3d, where x = horiz line along back wall of court, y =
     * vertical height (always set to current camera height), z =
     * distance from wall.
     */
    public ImageResults hoopset;

    public DataQueue() {
        hoopset = null;
    }

    public synchronized ImageResults get() {
        ImageResults tmp = hoopset;
        hoopset = null;
        return tmp;
    }

    public synchronized void put(ImageResults _hoopset) {
        hoopset = _hoopset;
    }
}
