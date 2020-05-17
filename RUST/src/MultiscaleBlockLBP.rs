use super::LBPBase::IBase;
extern crate image;
use image::{GrayImage, Luma, Pixel};

pub struct MultiscaleBlockLBP
{
    _scale : u32,
    _angle : u32,
}
impl MultiscaleBlockLBP
{
    pub fn new(scale : u32, angle : u32) -> MultiscaleBlockLBP
    {
        return MultiscaleBlockLBP{_scale : scale, _angle : angle};
    }
}
impl IBase for MultiscaleBlockLBP
{
    fn run_mutable(&self, src : &GrayImage) -> GrayImage
    {
        let (rows, cols) : (u32, u32) = src.dimensions();
        let cellsz : u32 = self._scale / 3;
        let cellar : u32 = cellsz * cellsz;
        let offset : u32 = cellsz / 2;

        let xCntRes = rows % cellsz;
        let xCntNum = if xCntRes > 0 {rows / cellsz + 1} else { rows / cellsz };

        let yCntRes = cols % cellsz;
        let yCntNum = if yCntRes > 0 {cols / cellsz + 1} else { cols / cellsz };

        if xCntNum < 3 || yCntNum < 3 || cellsz == 0
        {
            let mut dst = GrayImage::new(rows, cols);
            return dst;
        }

        let (mut xCurrentBlock, mut yCurrentBlock) : (u32, u32) = (0, 0);

        let mut avg_pixs : Vec<Vec<f32>> = vec![vec![0.0; yCntNum as usize]; xCntNum as usize];
        let mut lbp_pixs : Vec<Vec<u8>> = vec![vec![0u8; yCntNum as usize]; xCntNum as usize];

        for m in 0 .. xCntNum
        {
            for n in 0 .. yCntNum
            {
                xCurrentBlock = cellsz * m;
                yCurrentBlock = cellsz * n;
                if m == (xCntNum - 1) || n == (yCntNum - 1)
                {
                    let mut totalpix : u32 = 0;
                    let rowsL = if xCntRes == 0 {cellsz} else {xCntRes};
			        let colsL = if yCntRes == 0 {cellsz} else {yCntRes};
                    let resar = rowsL * colsL;
                    for i in 0 .. rowsL
                    {
                        for j in 0 .. colsL
                        {
                            let val : u8 = src.get_pixel(xCurrentBlock + i, yCurrentBlock + j).channels()[0];
                            totalpix += val as u32;
                        }
                    }
                    if resar > 0 
                    {
                        avg_pixs[m as usize][n as usize] = (totalpix as f32) / (resar as f32);}
                    }
                    else
                    {
                        let mut totalpix : u32 = 0;
                        for i in 0 .. cellsz
                        {
                            for j in 0 .. cellsz
                            {
                                let val : u8 = src.get_pixel(xCurrentBlock + i, yCurrentBlock + j).channels()[0];
                                totalpix += val as u32;
                            }
                        }
				        avg_pixs[m as usize][n as usize] = (totalpix as f32)/ (cellar as f32);
                    }
                }
            }

            let neighbor : u32 = 8;
            let angle : u32 = self._angle;
            let direction_x : [i32; 8] = [-1, -1, 0, 1, 1, 1, 0, -1 ];
            let direction_y : [i32; 8] = [0, 1, 1, 1, 0, -1, -1, -1 ];
            let mut idx : u32 = 0;
            let (mut dx, mut dy) : (i32, i32) = (0, 0);
            for k in 0 .. neighbor
            {
                idx = (k + angle) % neighbor;
                dx = direction_x[idx as usize];
                dy = direction_y[idx as usize];

                for m in 1 .. (xCntNum - 1)
                {
                    for n in 1 .. (yCntNum - 1)
                    {
                        let center : u8 = avg_pixs[m as usize][n as usize] as u8;
                        let px : usize = ((m as i32) + dx) as usize;
                        let py : usize = ((n as i32) + dy) as usize;
                        let edges : u8 = avg_pixs[px][py] as u8;
                        let signal : u8 = if center < edges { 1u8 } else { 0u8 };
                        lbp_pixs[m as usize][n as usize]  |= signal << (neighbor - k - 1);
                    }
                }
            }
        let mut dst = GrayImage::new(cellsz * (xCntNum - 2), cellsz * (yCntNum - 2));
        let (mut cur_rows, mut cur_cols) : (u32, u32) = (0, 0);
        for m in 1 .. (xCntNum - 1)
        {
            for n in 1 .. (yCntNum - 1)
            {
                let cur_rows = (m - 1) * cellsz;
                let cur_cols = (n - 1) * cellsz;
                for cur_r in 0 .. cellsz
                {
                    for cur_c in 0 .. cellsz
                    {
                        let dst_p = dst.get_pixel_mut(cur_r + cur_rows, cur_c + cur_cols).channels_mut();
                        dst_p[0] = lbp_pixs[m as usize][n as usize] as u8; 
                    }
                }
            }
        }
        return dst;
    }
}