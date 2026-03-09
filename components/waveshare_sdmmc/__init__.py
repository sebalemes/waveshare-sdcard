from esphome import automation

WRITE_ACTION_SCHEMA = cv.Schema({
    cv.Required("path"): cv.string,
    cv.Required("data"): cv.string,
})

READ_ACTION_SCHEMA = cv.Schema({
    cv.Required("path"): cv.string,
})

@automation.register_action("waveshare_sdmmc.write_file", WRITE_ACTION_SCHEMA)
async def write_file_action(config, action_id, template_arg):
    var = cg.new_Pvariable(action_id, template_arg)
    cg.add(var.set_path(config["path"]))
    cg.add(var.set_data(config["data"]))
    return var

@automation.register_action("waveshare_sdmmc.read_file", READ_ACTION_SCHEMA)
async def read_file_action(config, action_id, template_arg):
    var = cg.new_Pvariable(action_id, template_arg)
    cg.add(var.set_path(config["path"]))
    return var
