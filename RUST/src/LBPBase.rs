extern crate image;
use image::GrayImage;
pub trait IBase
{
    fn run_mutable(&self, src : &GrayImage) -> GrayImage;
    fn run_immutable(&self, src : &GrayImage) -> GrayImage
    {
        return self.run_mutable(src);
    }
    fn id() -> String
    {
        return String::from("IBase");
    }
}
// pub use image::GrayImage as GrayImage;