vim.cmd("set titlestring=PhoenixEngine")

function create_vim_proj_command(keybind, callback_func)
    vim.api.nvim_set_keymap("n", keybind, "", { noremap = true, silent = true, callback=callback_func });
end

function CompileEngine()
    vim.cmd("wa")
    vim.cmd("set makeprg=.\\engine\\build_engine.bat")
    vim.cmd("Make")
end

vim.api.nvim_create_user_command(
    'CompileEngine',
    CompileEngine,
    {bang = true}
)
create_vim_proj_command("<F8>", CompileEngine)

function CompileTestbed()
    vim.cmd("wa")
    vim.cmd("set makeprg=.\\testbed\\build_testbed.bat")
    vim.cmd("Make")
end

vim.api.nvim_create_user_command(
    'CompileTestbed',
    CompileTestbed,
    {bang = true}
)

create_vim_proj_command("<F9>", CompileTestbed)

function PostBuild()
    vim.cmd("wa")
    vim.cmd("set makeprg=.\\post_build.bat")
    vim.cmd("Make")
end

vim.api.nvim_create_user_command(
    'PostBuild',
    PostBuild,
    {bang = true}
)

create_vim_proj_command("<F10>", PostBuild)

function CompileAll()
    vim.cmd("wa")
    vim.cmd("set makeprg=.\\build_all.bat")
    vim.cmd("Make")
end

vim.api.nvim_create_user_command(
    'CompileAll',
    CompileAll,
    {bang = true}
)

create_vim_proj_command("<F11>", CompileAll)
