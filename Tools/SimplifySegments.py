import math


class SegmentEndpoint():
    ''' Object representing the endpoint of a segment of the drawing, where
    each segment has two cooresponding SegmentEndpoints (one each for the
    start and end).
    '''
    # The coordinate pair for the endpoint
    point: tuple
    # The index of the corresponding segment in the segment list
    seg_idx: int
    # If the point is the start of the segment
    isstart: bool
    # The index of the other endpoint in the endpoint list
    other_ep_idx: int

    def __init__(self, point, idx, isstart, other_idx):
        self.point = point
        self.seg_idx = idx
        self.isstart = isstart
        self.other_ep_idx = other_idx


def euclidian_distance(a: tuple, b: tuple):
    x_dist: float = a[0] - b[0]
    y_dist: float = a[1] - b[1]
    return math.sqrt(x_dist**2 + y_dist**2)


def remove_space(segments: list, endpoints: list, margin: int):
    ''' Removes space between segments less than the margin. Combines segments
    with similar endpoints into a single longer segment.
    '''
    # Compare all endpoints to see if distance is similar
    for i in range(len(endpoints)-1):
        ep1 = endpoints[i]
        if ep1 is None:
            continue
        for j in range(i+1, len(endpoints)):
            ep2 = endpoints[j]
            if ep2 is None:
                continue
            if euclidian_distance(ep1.point, ep2.point) <= margin:
                # Dont try to combine endpoints of same segment
                if ep1.seg_idx == ep2.seg_idx:
                    continue
                # Combine segments, so that ep2 is appended to ep1
                # Handle segment 1
                new_start = endpoints[ep1.other_ep_idx]
                new_start.isstart = True
                new_start.other_ep_idx = ep2.other_ep_idx
                if ep1.isstart:
                    # Must flip segment1 so that ep1 is the end
                    segments[ep1.seg_idx].reverse()  # Reverse the segment
                # Handle segment 2
                new_end = endpoints[ep2.other_ep_idx]
                new_end.seg_idx = new_start.seg_idx
                new_end.isstart = False
                new_end.other_ep_idx = ep1.other_ep_idx
                if not ep2.isstart:
                    # Must flip segment2 so that ep2 is the start
                    segments[ep2.seg_idx].reverse()
                # Combine the segments
                segments[ep1.seg_idx].extend(segments[ep2.seg_idx])
                segments[ep2.seg_idx] = None  # Remove the segment
                endpoints[i] = None  # Remove old end point for segment1
                endpoints[j] = None  # Remove old start point for segment2
                break


def find_closest(endpoints: list, refrence_point: tuple, ignore_idx: int) -> int:
    ''' Finds the closest endpoint in the list of endpoints, returning 
    the index of the closest endpoint.
    Args:
        endpoints - the list of SegmentEndpoint objects
        refrence_point - the point to compare for distance
        ignore_idx - an index in the list of endpoints to ignore (the index
                     of the refrence_point)
    Returns the index of the closest endpoint in the list of endpoints
    '''
    cur_min_val = -1
    cur_min_idx = -1
    for j in range(len(endpoints)):
        if j == ignore_idx or endpoints[j] is None:
            continue
        this_dist = euclidian_distance(refrence_point, endpoints[j].point)
        if cur_min_val < 0 or this_dist < cur_min_val:
            cur_min_val = this_dist
            cur_min_idx = j
    return cur_min_idx


def organize_by_distance(segments: list, endpoints: list):
    ''' Organizes segments by their distance to eachother. Finds the 
    endpoint closest to the origin, then traces a path through all line segments
    finding the shortest distance to the next segment.
    Returns a list of segments organized by distance
    '''

    prev_idx = -1
    point = (0, 0)
    organized_segments = []
    while True:
        next_ep_idx = find_closest(endpoints, point, prev_idx)
        if next_ep_idx == -1: break
        next_ep = endpoints[next_ep_idx]
        if not next_ep.isstart:
            # Flip the segments before appending
            segments[next_ep.seg_idx].reverse()
        organized_segments.append(segments[next_ep.seg_idx])  # Add the segment to new list
        segments[next_ep.seg_idx] = None  # Remove from unorganized segments list
        endpoints[next_ep_idx] = None # Remove start point
        point = endpoints[next_ep.other_ep_idx].point  # Update point for next iteration
        endpoints[next_ep.other_ep_idx] = None  # Remove end point
    assert(all(x is None for x in segments)), f"Not all segments removed\n{segments}"
    assert(all(x is None for x in endpoints)), f"Not all endpoints removed\n{endpoints}"
    return organized_segments



def simplify_segments(segments: list, margin: int = 10) -> list:
    ''' Takes a list of line segments and simplifies the list, including 
    organizing them to minimize travel time between segments and removing small 
    amounts of space in between.
    Args:
        segments - the list of line segments, where each segment is a list of 
                    continuous coordinate points
        margin - the allowed margin between the segment endpoints, before the
                    seperate segments are combined into a single continuous 
                    segment
    Returns: the simplified list of segments
    '''
    # Create mapping of each endpoint to each obj
    endpoints: list = []  # The list of endpoints
    for i in range(len(segments)):
        seg = segments[i]
        assert(len(seg) > 0)
        if len(seg) < 2:
            endpoints.append(SegmentEndpoint(seg[0], i, True, len(endpoints)))
        else:
            first_endpoint_idx = len(endpoints)
            endpoints.append(SegmentEndpoint(
                seg[0], i, True, first_endpoint_idx+1))
            endpoints.append(SegmentEndpoint(
                seg[-1], i, False, first_endpoint_idx))
    remove_space(segments, endpoints, margin)
    return organize_by_distance(segments, endpoints)
