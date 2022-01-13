import sys, os
import cv2

def overlapImages(bmp_file: str, jpg_file: str):
    tst_img = cv2.imread(bmp_file)
    assert(tst_img is not None), f"Couldn't open test image {bmp_file}"
    print(f"Test shape {tst_img.shape}")
    viz_img = cv2.imread(jpg_file)
    assert(viz_img is not None), f"Couldn't open viz image {jpg_file}"
    print(f"Viz shape {viz_img.shape}")
    assert(viz_img.shape == tst_img.shape), "Image shapes don't match viz {viz_img.shape} test {tst_img.shape}"
    combo = tst_img + viz_img

    alpha = 0.5
    beta = (1.0 - alpha)
    combo = cv2.addWeighted(tst_img, alpha, viz_img, beta, 0.0)
    # cv2.imshow("overlapped", combo)
    cv2.waitKey(0)
    if cv2.imwrite("out/overlapped.bmp", combo):
    # if cv2.imwrite("overlapped.jpg", combo):
        print(f"Saved overlapping image {combo.shape[0]}px x {combo.shape[1]}px")
    else:
        print("Failed to save image")



if __name__ == '__main__':
    '''
    Overlaps the viz JPG with the testing BMP to show differences
    '''

    # Check correct arguments
    if (len(sys.argv) < 3):
        print('Usage: python OverlapVizTest.py path\\to\\file.bmp path\\to\\file.jpg')
        exit()

    bmp_file: str = sys.argv[1]
    jpg_file: str = sys.argv[2]

    print(f"Overlaying {bmp_file}")
    print(f"       and {jpg_file}")

    overlapImages(bmp_file, jpg_file)

