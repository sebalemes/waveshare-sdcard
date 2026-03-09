import esphome.codegen as cg
import esphome.config_validation as cv

waveshare_sdmmc_ns = cg.esphome_ns.namespace("waveshare_sdmmc")
Waveshare_SDMMC = waveshare_sdmmc_ns.class_("WaveshareSDMMC", cg.Component)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(Waveshare_SDMMC),
})

async def to_code(config):
    var = cg.new_Pvariable(config[cv.GenerateID()])
    await cg.register_component(var, config)
